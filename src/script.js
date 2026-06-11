const API_BASE = 'http://localhost:8080/api';
let isLoginMode = true;

// DOM Elements
const authForm       = document.getElementById('authForm');
const authToggleBtn  = document.getElementById('authToggleBtn');
const dateInput      = document.getElementById('journalDate');
const productModal   = document.getElementById('productModal');
const addProductForm = document.getElementById('addProductForm');
const alertModal     = document.getElementById('alertModal');

// Init
document.addEventListener('DOMContentLoaded', () => {
    dateInput.value = new Date().toISOString().split('T')[0];
    dateInput.addEventListener('change', loadJournal);

    const savedUserId = localStorage.getItem('userId');
    if (savedUserId) {
        switchTab('profile');
    } else {
        switchTab('auth');
    }
});

// Routing
function switchTab(tabId) {
    const mainLayout = document.getElementById('mainLayout');
    const authScreen = document.getElementById('authScreen');
    const sections   = ['profile', 'journal', 'stats', 'about'];

    if (tabId === 'auth') {
        authScreen.classList.remove('hidden');
        mainLayout.classList.add('hidden');
    } else {
        authScreen.classList.add('hidden');
        mainLayout.classList.remove('hidden');

        sections.forEach(sec => {
            const el = document.getElementById(sec);
            if (el) el.classList.toggle('hidden', sec !== tabId);
        });

        if (tabId === 'profile') loadProfile();
        if (tabId === 'journal') loadJournal();
        if (tabId === 'stats')   loadStats();    // ← НОВИЙ виклик
    }
}

// Auth Logic
authToggleBtn.addEventListener('click', () => {
    isLoginMode = !isLoginMode;
    document.getElementById('authTitle').innerText      = isLoginMode ? 'Вхід' : 'Реєстрація';
    document.getElementById('authSubmitBtn').innerText  = isLoginMode ? 'Увійти' : 'Зареєструватись';
    document.getElementById('authToggleText').innerText = isLoginMode ? 'Немає акаунту?' : 'Вже є акаунт?';
    authToggleBtn.innerText = isLoginMode ? 'Зареєструватись' : 'Увійти';
    document.getElementById('registerFields').classList.toggle('hidden', isLoginMode);
});

authForm.addEventListener('submit', async (e) => {
    e.preventDefault();

    const email    = document.getElementById('authEmail').value;
    const password = document.getElementById('authPassword').value;
    const url      = `${API_BASE}/${isLoginMode ? 'login' : 'register'}`;
    const payload  = { email, password };

    if (!isLoginMode) {
        payload.name           = document.getElementById('regName').value;
        payload.gender         = document.getElementById('regGender').value;
        payload.age            = parseInt(document.getElementById('regAge').value)    || 0;
        payload.height         = parseFloat(document.getElementById('regHeight').value) || 0;
        payload.weight         = parseFloat(document.getElementById('regWeight').value) || 0;
        payload.activity_level = document.getElementById('regActivity').value;
        payload.goal           = document.querySelector('input[name="goal"]:checked')?.value || 'maintain';
    }

    try {
        const res = await fetch(url, {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify(payload)
        });
        if (!res.ok) throw new Error('Авторизація не вдалася');

        const data = await res.json();
        localStorage.setItem('userId', data.id);

        authForm.reset();
        isLoginMode = true;
        switchTab('profile');
    } catch (err) {
        openAlert('Помилка: Перевірте введені дані', true);
    }
});

document.getElementById('logoutBtn').addEventListener('click', () => {
    localStorage.removeItem('userId');
    switchTab('auth');
});

// ── Load Profile ─────────────────────────────────────────────────────────────
async function loadProfile() {
    const userId = localStorage.getItem('userId');
    if (!userId) return switchTab('auth');

    try {
        const res = await fetch(`${API_BASE}/profile?user_id=${userId}`);
        if (!res.ok) throw new Error('Не вдалося завантажити профіль');

        const data = await res.json();

        document.getElementById('profInitial').innerText  = (data.name || 'U').charAt(0).toUpperCase();
        document.getElementById('profAge').innerText      = data.age    || '-';
        document.getElementById('profGender').innerText   = data.gender === 'male' ? 'Чоловік' : 'Жінка';
        document.getElementById('profHeight').innerText   = data.height || '-';
        document.getElementById('profWeight').innerText   = data.weight || '-';

        const goalsMap = { 'lose': 'Схуднення', 'maintain': 'Підтримка', 'gain': 'Набір маси' };
        document.getElementById('profGoal').innerText = goalsMap[data.goal] || '-';

        const limit    = data.dailyLimit    || 2200;
        const consumed = data.consumedToday || 0;

        document.getElementById('profLimitText').innerText    = limit;
        document.getElementById('profConsumedText').innerText = consumed;

        const percent = Math.min((consumed / limit) * 100, 100);
        document.getElementById('profProgressBar').style.width = `${percent}%`;

    } catch (err) {
        console.warn('API /profile помилка:', err.message);
    }
}

// ── Load Journal ─────────────────────────────────────────────────────────────
async function loadJournal() {
    const userId = localStorage.getItem('userId');
    if (!userId) return;

    const date  = dateInput.value;
    const tbody = document.getElementById('journalTableBody');
    tbody.innerHTML = '<tr><td colspan="5" class="p-6 text-center text-emerald-500 font-medium">Завантаження...</td></tr>';

    try {
        const res = await fetch(`${API_BASE}/journal?user_id=${userId}&date=${date}`);
        if (!res.ok) throw new Error();
        const products = await res.json();

        tbody.innerHTML = '';

        if (products.length === 0) {
            tbody.innerHTML = `<tr><td colspan="5" class="p-6 text-center text-emerald-500 font-medium">Записів на цю дату немає</td></tr>`;
            return;
        }

        products.forEach(p => {
            tbody.innerHTML += `
                <tr class="border-b border-emerald-800/30 hover:bg-emerald-800/20 transition duration-200">
                    <td class="p-4 text-emerald-100 font-medium">${p.name}</td>
                    <td class="p-4 text-emerald-300">${p.weight}</td>
                    <td class="p-4 font-bold text-emerald-400">${p.calories}</td>
                    <td class="p-4 text-emerald-300">${p.category}</td>
                    <td class="p-4 text-emerald-500">${p.time}</td>
                </tr>
            `;
        });

        loadProfile();
    } catch (err) {
        tbody.innerHTML = `<tr><td colspan="5" class="p-6 text-center text-red-500 font-medium">Помилка завантаження</td></tr>`;
    }
}

// ── НОВИЙ: Load Stats ─────────────────────────────────────────────────────────
async function loadStats() {
    const userId = localStorage.getItem('userId');
    if (!userId) return;

    const container = document.getElementById('statsContent');
    container.innerHTML = '<p class="text-emerald-500 text-center py-8">Завантаження...</p>';

    try {
        const [statsRes, profileRes] = await Promise.all([
            fetch(`${API_BASE}/stats?user_id=${userId}`),
            fetch(`${API_BASE}/profile?user_id=${userId}`)
        ]);

        const days    = await statsRes.json();
        const profile = await profileRes.json();
        const limit   = profile.dailyLimit || 2200;

        if (!days || days.length === 0) {
            container.innerHTML = '<p class="text-emerald-500 text-center py-8">Немає даних. Додайте продукти у журнал.</p>';
            return;
        }

        // Сортуємо за датою і беремо останні 7 днів
        days.sort((a, b) => a.date.localeCompare(b.date));
        const recent = days.slice(-7);

        const maxCal = Math.max(...recent.map(d => d.calories), limit);

        let rows = '';
        recent.forEach(d => {
            const pct     = Math.min((d.calories / maxCal) * 100, 100);
            const barColor = d.calories > limit ? 'bg-red-500' : 'bg-emerald-500';
            const label    = d.date.slice(5); // MM-DD
            rows += `
                <div class="flex items-center gap-4">
                    <span class="text-emerald-400 w-14 text-sm font-mono">${label}</span>
                    <div class="flex-1 bg-gray-800 rounded-full h-7 overflow-hidden">
                        <div class="${barColor} h-7 rounded-full transition-all duration-700 flex items-center pl-3"
                             style="width:${pct}%">
                            <span class="text-xs font-bold text-white whitespace-nowrap">${Math.round(d.calories)} ккал</span>
                        </div>
                    </div>
                </div>
            `;
        });

        container.innerHTML = `
            <div class="space-y-4">
                <div class="flex items-center gap-4 text-sm text-gray-400 mb-2">
                    <span class="w-14"></span>
                    <span>Норма: <strong class="text-emerald-400">${limit} ккал</strong></span>
                    <span class="flex items-center gap-1"><span class="inline-block w-3 h-3 rounded-full bg-emerald-500"></span> В нормі</span>
                    <span class="flex items-center gap-1"><span class="inline-block w-3 h-3 rounded-full bg-red-500"></span> Перевищення</span>
                </div>
                ${rows}
            </div>
        `;
    } catch (err) {
        container.innerHTML = '<p class="text-red-500 text-center py-8">Помилка завантаження статистики</p>';
    }
}

// ── НОВИЙ: Search catalog ─────────────────────────────────────────────────────
let searchTimeout = null;

async function onSearchInput(e) {
    const query = e.target.value.trim();
    const resultsBox = document.getElementById('searchResults');

    if (query.length < 1) {
        resultsBox.classList.add('hidden');
        resultsBox.innerHTML = '';
        return;
    }

    clearTimeout(searchTimeout);
    searchTimeout = setTimeout(async () => {
        try {
            const res = await fetch(`${API_BASE}/catalog?query=${encodeURIComponent(query)}`);
            const items = await res.json();

            if (!items || items.length === 0) {
                resultsBox.innerHTML = '<div class="p-3 text-emerald-500 text-sm">Нічого не знайдено</div>';
                resultsBox.classList.remove('hidden');
                return;
            }

            resultsBox.innerHTML = items.map(item => `
                <div class="p-3 hover:bg-emerald-800/40 cursor-pointer border-b border-gray-700 last:border-0 transition"
                     onclick="selectFood(${item.id}, '${item.name.replace(/'/g,"\\'")}', ${item.caloriesPer100g}, '${item.category}')">
                    <div class="font-semibold text-emerald-100">${item.name}</div>
                    <div class="text-xs text-emerald-400">${item.caloriesPer100g} ккал/100г &nbsp;·&nbsp; Б: ${item.protein}г &nbsp;·&nbsp; Ж: ${item.fat}г &nbsp;·&nbsp; В: ${item.carbs}г</div>
                </div>
            `).join('');
            resultsBox.classList.remove('hidden');

        } catch (err) {
            console.warn('Пошук недоступний:', err.message);
        }
    }, 300);
}

// НОВИЙ: Вибір продукту з результатів пошуку — заповнює форму і рахує калорії
function selectFood(id, name, calPer100g, category) {
    document.getElementById('prodName').value = name;
    document.getElementById('prodCat').value  = category;
    document.getElementById('searchResults').classList.add('hidden');
    document.getElementById('foodSearch').value = '';

    // Зберігаємо калорійність/100г для перерахунку
    document.getElementById('prodWeight')._calPer100g = calPer100g;

    // Якщо вага вже введена — одразу перераховуємо
    recalcCalories();
}

// НОВИЙ: Автоматичний перерахунок калорій при зміні ваги
function recalcCalories() {
    const weightInput = document.getElementById('prodWeight');
    const calInput    = document.getElementById('prodCal');
    const weight      = parseFloat(weightInput.value);
    const calPer100g  = weightInput._calPer100g;

    if (calPer100g && weight > 0) {
        calInput.value = Math.round((calPer100g * weight) / 100);
    }
}

// ── Add Product ───────────────────────────────────────────────────────────────
document.getElementById('addProductForm').addEventListener('submit', async (e) => {
    e.preventDefault();
    const userId = localStorage.getItem('userId');

    const payload = {
        user_id:  parseInt(userId),
        name:     document.getElementById('prodName').value,
        weight:   parseFloat(document.getElementById('prodWeight').value),
        calories: parseFloat(document.getElementById('prodCal').value),
        category: document.getElementById('prodCat').value,
        time:     document.getElementById('prodTime').value,
        date:     dateInput.value
    };

    try {
        const res = await fetch(`${API_BASE}/journal/add`, {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify(payload)
        });
        if (res.ok) {
            closeProductModal();
            addProductForm.reset();
            // очищаємо збережене calPer100g
            document.getElementById('prodWeight')._calPer100g = null;
            openAlert('Продукт успішно додано!', false);
            loadJournal();
        } else {
            throw new Error();
        }
    } catch (err) {
        openAlert('Помилка додавання продукту', true);
    }
});

// ── Modal Interactions ────────────────────────────────────────────────────────
function openProductModal() {
    productModal.classList.remove('hidden');
    document.getElementById('prodTime').value = new Date().toTimeString().slice(0, 5);
    document.getElementById('searchResults').classList.add('hidden');
}

function closeProductModal() {
    productModal.classList.add('hidden');
    document.getElementById('prodWeight')._calPer100g = null;
}

function openAlert(msg, isError = false) {
    const box  = document.getElementById('alertBox');
    const text = document.getElementById('alertMsg');

    text.innerText = msg;
    if (isError) {
        box.classList.replace('border-emerald-500', 'border-red-500') || box.classList.add('border-red-500');
        text.className = 'text-xl font-bold mb-6 text-red-400';
    } else {
        box.classList.replace('border-red-500', 'border-emerald-500') || box.classList.add('border-emerald-500');
        text.className = 'text-xl font-bold mb-6 text-emerald-400';
    }
    alertModal.classList.remove('hidden');
}

function closeAlert() {
    alertModal.classList.add('hidden');
}
