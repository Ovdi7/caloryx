const API_BASE = 'http://localhost:8080/api';
let isLoginMode = true;

const authForm       = document.getElementById('authForm');
const authToggleBtn  = document.getElementById('authToggleBtn');
const dateInput      = document.getElementById('journalDate');
const productModal   = document.getElementById('productModal');
const addProductForm = document.getElementById('addProductForm');
const alertModal     = document.getElementById('alertModal');

// ── Init ──────────────────────────────────────────────────────────────────────
document.addEventListener('DOMContentLoaded', () => {
    dateInput.value = new Date().toISOString().split('T')[0];
    dateInput.addEventListener('change', loadJournal);

    if (localStorage.getItem('userId')) switchTab('profile');
    else switchTab('auth');
});

// ── Routing ───────────────────────────────────────────────────────────────────
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
            document.getElementById(sec)?.classList.toggle('hidden', sec !== tabId);
        });
        if (tabId === 'profile') loadProfile();
        if (tabId === 'journal') loadJournal();
        if (tabId === 'stats')   loadStats();
    }
}

// ── Auth ──────────────────────────────────────────────────────────────────────
authToggleBtn.addEventListener('click', () => {
    isLoginMode = !isLoginMode;
    document.getElementById('authTitle').innerText      = isLoginMode ? 'Вхід' : 'Реєстрація';
    document.getElementById('authSubmitBtn').innerText  = isLoginMode ? 'Увійти' : 'Зареєструватись';
    document.getElementById('authToggleText').innerText = isLoginMode ? 'Немає акаунту?' : 'Вже є акаунт?';
    authToggleBtn.innerText = isLoginMode ? 'Зареєструватись' : 'Увійти';
    document.getElementById('registerFields').classList.toggle('hidden', isLoginMode);
});

// Показати/приховати пароль
function togglePassword() {
    const inp = document.getElementById('authPassword');
    const btn = document.getElementById('togglePassBtn');
    if (inp.type === 'password') { inp.type = 'text';     btn.innerText = '🙈'; }
    else                         { inp.type = 'password'; btn.innerText = '👁'; }
}

authForm.addEventListener('submit', async (e) => {
    e.preventDefault();
    const email    = document.getElementById('authEmail').value;
    const password = document.getElementById('authPassword').value;
    const url      = `${API_BASE}/${isLoginMode ? 'login' : 'register'}`;
    const payload  = { email, password };

    if (!isLoginMode) {
        payload.name           = document.getElementById('regName').value;
        payload.gender         = document.getElementById('regGender').value;
        payload.birth_year     = parseInt(document.getElementById('regBirthYear').value) || 2000;
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
        if (!res.ok) throw new Error();
        const data = await res.json();
        localStorage.setItem('userId', data.id);
        authForm.reset();
        isLoginMode = true;
        switchTab('profile');
    } catch {
        openAlert('Помилка: Перевірте введені дані', true);
    }
});

document.getElementById('logoutBtn').addEventListener('click', () => {
    localStorage.removeItem('userId');
    switchTab('auth');
});

// ── Load Profile ──────────────────────────────────────────────────────────────
async function loadProfile() {
    const userId = localStorage.getItem('userId');
    if (!userId) return switchTab('auth');
    try {
        const res  = await fetch(`${API_BASE}/profile?user_id=${userId}`);
        if (!res.ok) throw new Error();
        const data = await res.json();

        document.getElementById('profInitial').innerText  = (data.name || 'U').charAt(0).toUpperCase();
        document.getElementById('profAge').innerText      = data.age    || '-';
        document.getElementById('profGender').innerText   = data.gender === 'male' ? 'Чоловік' : 'Жінка';
        document.getElementById('profHeight').innerText   = data.height || '-';
        document.getElementById('profWeight').innerText   = data.weight || '-';
        document.getElementById('profActivity').innerText = activityLabel(data.activity_level);

        const goalsMap = { lose: 'Схуднення', maintain: 'Підтримка', gain: 'Набір маси' };
        document.getElementById('profGoal').innerText = goalsMap[data.goal] || '-';

        const limit    = data.dailyLimit    || 2200;
        const consumed = Math.round(data.consumedToday || 0);
        document.getElementById('profLimitText').innerText    = limit;
        document.getElementById('profConsumedText').innerText = consumed;
        const pct = Math.min((consumed / limit) * 100, 100);
        document.getElementById('profProgressBar').style.width = `${pct}%`;
        document.getElementById('profProgressBar').className =
            `h-6 rounded-full transition-all duration-700 ease-out ${pct >= 100 ? 'bg-red-500' : 'bg-emerald-500'}`;

        // БЖВ за сьогодні
        document.getElementById('profProtein').innerText = Math.round(data.proteinToday || 0);
        document.getElementById('profFat').innerText     = Math.round(data.fatToday     || 0);
        document.getElementById('profCarbs').innerText   = Math.round(data.carbsToday   || 0);

        // Заповнити форму редагування
        document.getElementById('editName').value       = data.name           || '';
        document.getElementById('editHeight').value     = data.height         || '';
        document.getElementById('editWeight').value     = data.weight         || '';
        document.getElementById('editBirthYear').value  = data.birth_year     || '';
        document.getElementById('editActivity').value   = data.activity_level || 'sedentary';
        const goalRadio = document.querySelector(`input[name="editGoal"][value="${data.goal}"]`);
        if (goalRadio) goalRadio.checked = true;

    } catch (err) {
        console.warn('Помилка профілю:', err);
    }
}

function activityLabel(v) {
    return { sedentary: 'Мінімальна', light: 'Легка', moderate: 'Помірна', active: 'Активна' }[v] || '-';
}

// ── Редагування профілю ───────────────────────────────────────────────────────
function openEditModal() {
    document.getElementById('editModal').classList.remove('hidden');
}
function closeEditModal() {
    document.getElementById('editModal').classList.add('hidden');
}

document.getElementById('editProfileForm').addEventListener('submit', async (e) => {
    e.preventDefault();
    const userId = localStorage.getItem('userId');
    const payload = {
        user_id:        parseInt(userId),
        name:           document.getElementById('editName').value,
        height:         parseFloat(document.getElementById('editHeight').value),
        weight:         parseFloat(document.getElementById('editWeight').value),
        birth_year:     parseInt(document.getElementById('editBirthYear').value),
        activity_level: document.getElementById('editActivity').value,
        goal:           document.querySelector('input[name="editGoal"]:checked')?.value || 'maintain',
    };
    try {
        const res = await fetch(`${API_BASE}/profile/update`, {
            method: 'PUT',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify(payload)
        });
        if (!res.ok) throw new Error();
        closeEditModal();
        openAlert('Профіль оновлено!', false);
        loadProfile();
    } catch {
        openAlert('Помилка збереження', true);
    }
});

// ── Load Journal ──────────────────────────────────────────────────────────────
async function loadJournal() {
    const userId = localStorage.getItem('userId');
    if (!userId) return;
    const date  = dateInput.value;
    const tbody = document.getElementById('journalTableBody');
    const summary = document.getElementById('journalSummary');
    tbody.innerHTML = '<tr><td colspan="7" class="p-6 text-center text-emerald-500">Завантаження...</td></tr>';
    summary.classList.add('hidden');

    try {
        const res      = await fetch(`${API_BASE}/journal?user_id=${userId}&date=${date}`);
        if (!res.ok) throw new Error();
        const products = await res.json();

        tbody.innerHTML = '';
        if (products.length === 0) {
            tbody.innerHTML = `<tr><td colspan="7" class="p-6 text-center text-emerald-500">Записів на цю дату немає</td></tr>`;
            return;
        }

        let totalCal = 0, totalProt = 0, totalFat = 0, totalCarbs = 0;
        products.forEach(p => {
            totalCal   += p.calories || 0;
            totalProt  += p.protein  || 0;
            totalFat   += p.fat      || 0;
            totalCarbs += p.carbs    || 0;
            tbody.innerHTML += `
                <tr class="border-b border-emerald-800/30 hover:bg-emerald-800/20 transition">
                    <td class="p-4 text-emerald-100 font-medium">${p.name}</td>
                    <td class="p-4 text-emerald-300">${p.weight} г</td>
                    <td class="p-4 font-bold text-emerald-400">${Math.round(p.calories)}</td>
                    <td class="p-4 text-blue-300">${Math.round(p.protein || 0)}г</td>
                    <td class="p-4 text-yellow-300">${Math.round(p.fat || 0)}г</td>
                    <td class="p-4 text-orange-300">${Math.round(p.carbs || 0)}г</td>
                    <td class="p-4">
                        <button onclick="deleteProduct(${p.id})"
                                class="text-red-400 hover:text-red-300 text-xs font-bold px-2 py-1 rounded border border-red-700 hover:bg-red-900/30 transition">
                            ✕
                        </button>
                    </td>
                </tr>`;
        });

        // Підсумок за день
        summary.classList.remove('hidden');
        document.getElementById('sumCal').innerText   = Math.round(totalCal);
        document.getElementById('sumProt').innerText  = Math.round(totalProt);
        document.getElementById('sumFat').innerText   = Math.round(totalFat);
        document.getElementById('sumCarbs').innerText = Math.round(totalCarbs);

        loadProfile();
    } catch {
        tbody.innerHTML = `<tr><td colspan="7" class="p-6 text-center text-red-500">Помилка завантаження</td></tr>`;
    }
}

// ── Видалення запису ──────────────────────────────────────────────────────────
async function deleteProduct(id) {
    const userId = localStorage.getItem('userId');
    try {
        const res = await fetch(`${API_BASE}/journal/delete`, {
            method: 'DELETE',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify({ id, user_id: parseInt(userId) })
        });
        if (!res.ok) throw new Error();
        loadJournal();
    } catch {
        openAlert('Помилка видалення', true);
    }
}

// ── Load Stats ────────────────────────────────────────────────────────────────
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

        days.sort((a, b) => a.date.localeCompare(b.date));
        const recent = days.slice(-7);
        const maxCal = Math.max(...recent.map(d => d.calories), limit);

        let rows = '';
        recent.forEach(d => {
            const pct      = Math.min((d.calories / maxCal) * 100, 100);
            const barColor = d.calories > limit ? 'bg-red-500' : 'bg-emerald-500';
            rows += `
                <div class="flex items-center gap-4">
                    <span class="text-emerald-400 w-16 text-sm font-mono">${d.date.slice(5)}</span>
                    <div class="flex-1 bg-gray-800 rounded-full h-7 overflow-hidden">
                        <div class="${barColor} h-7 rounded-full flex items-center pl-3"
                             style="width:${pct}%; transition: width .7s">
                            <span class="text-xs font-bold text-white whitespace-nowrap">${Math.round(d.calories)} ккал</span>
                        </div>
                    </div>
                </div>`;
        });

        container.innerHTML = `
            <div class="space-y-4">
                <div class="flex items-center gap-4 text-sm text-gray-400 mb-2">
                    <span class="w-16"></span>
                    <span>Норма: <strong class="text-emerald-400">${limit} ккал</strong></span>
                    <span class="flex items-center gap-1"><span class="w-3 h-3 rounded-full bg-emerald-500 inline-block"></span> В нормі</span>
                    <span class="flex items-center gap-1"><span class="w-3 h-3 rounded-full bg-red-500 inline-block"></span> Перевищення</span>
                </div>
                ${rows}
            </div>`;
    } catch {
        container.innerHTML = '<p class="text-red-500 text-center py-8">Помилка завантаження статистики</p>';
    }
}

// ── Пошук продуктів ───────────────────────────────────────────────────────────
let searchTimeout = null;

// UTF-8 lowercase кирилиці на фронті
function cyrillicLower(str) {
    return str.replace(/[\u0400-\u042F\u0404\u0406\u0407]/g, c => c.toLowerCase());
}

async function onSearchInput(e) {
    const query      = cyrillicLower(e.target.value.trim());
    const resultsBox = document.getElementById('searchResults');
    if (query.length < 1) { resultsBox.classList.add('hidden'); return; }

    clearTimeout(searchTimeout);
    searchTimeout = setTimeout(async () => {
        try {
            const res   = await fetch(`${API_BASE}/catalog?query=${encodeURIComponent(query)}`);
            const items = await res.json();

            if (!items || items.length === 0) {
                resultsBox.innerHTML = '<div class="p-3 text-emerald-500 text-sm">Нічого не знайдено</div>';
                resultsBox.classList.remove('hidden');
                return;
            }

            resultsBox.innerHTML = items.map(item => `
                <div class="p-3 hover:bg-emerald-800/40 cursor-pointer border-b border-gray-700 last:border-0 transition"
                     onclick="selectFood(${item.id},'${item.name.replace(/'/g,"\\'")}',${item.caloriesPer100g},'${item.category}',${item.protein},${item.fat},${item.carbs})">
                    <div class="font-semibold text-emerald-100">${item.name}</div>
                    <div class="text-xs text-emerald-400">${item.caloriesPer100g} ккал/100г &nbsp;·&nbsp; Б:${item.protein}г Ж:${item.fat}г В:${item.carbs}г</div>
                </div>`).join('');
            resultsBox.classList.remove('hidden');
        } catch (err) {
            console.warn('Пошук:', err.message);
        }
    }, 300);
}

function selectFood(id, name, calPer100g, category, protein, fat, carbs) {
    document.getElementById('prodName').value = name;
    document.getElementById('prodCat').value  = category;
    document.getElementById('searchResults').classList.add('hidden');
    document.getElementById('foodSearch').value = '';

    const w = document.getElementById('prodWeight');
    w._calPer100g = calPer100g;
    w._protein    = protein;
    w._fat        = fat;
    w._carbs      = carbs;

    recalcCalories();
}

function recalcCalories() {
    const w          = document.getElementById('prodWeight');
    const calInput   = document.getElementById('prodCal');
    const weight     = parseFloat(w.value);
    if (w._calPer100g && weight > 0) {
        calInput.value = Math.round(w._calPer100g * weight / 100);
    }
}

// ── Add Product ───────────────────────────────────────────────────────────────
document.getElementById('addProductForm').addEventListener('submit', async (e) => {
    e.preventDefault();
    const userId = localStorage.getItem('userId');
    const w      = document.getElementById('prodWeight');
    const weight = parseFloat(w.value);
    const factor = (w._calPer100g && weight > 0) ? weight / 100 : 0;

    const payload = {
        user_id:  parseInt(userId),
        name:     document.getElementById('prodName').value,
        weight,
        calories: parseFloat(document.getElementById('prodCal').value),
        category: document.getElementById('prodCat').value,
        time:     document.getElementById('prodTime').value,
        date:     dateInput.value,
        protein:  factor ? Math.round(w._protein * factor * 10) / 10 : 0,
        fat:      factor ? Math.round(w._fat     * factor * 10) / 10 : 0,
        carbs:    factor ? Math.round(w._carbs   * factor * 10) / 10 : 0,
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
            w._calPer100g = null;
            openAlert('Продукт успішно додано!', false);
            loadJournal();
        } else throw new Error();
    } catch {
        openAlert('Помилка додавання продукту', true);
    }
});

// ── Modals ────────────────────────────────────────────────────────────────────
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
function closeAlert() { alertModal.classList.add('hidden'); }
