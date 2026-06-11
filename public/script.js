// ─── Стан ────────────────────────────────────────────────────────────────────
const API = 'http://localhost:8080/api';

const state = {
    user: JSON.parse(localStorage.getItem('user') || 'null'),
    currentDate: new Date().toISOString().split('T')[0],
    selectedProduct: null   // { name, calories, protein, fats, carbs, category }
};

// ─── Авторизація / переключення екранів ──────────────────────────────────────
let isRegisterMode = false;

function showScreen(name) {
    document.getElementById('authScreen').classList.toggle('hidden', name !== 'auth');
    document.getElementById('mainLayout').classList.toggle('hidden', name !== 'main');
}

function initApp() {
    document.getElementById('journalDate').value = state.currentDate;
    if (state.user) {
        showScreen('main');
        switchTab('profile');
    } else {
        showScreen('auth');
    }
}

document.getElementById('authToggleBtn').addEventListener('click', () => {
    isRegisterMode = !isRegisterMode;
    document.getElementById('registerFields').classList.toggle('hidden', !isRegisterMode);
    document.getElementById('authTitle').textContent    = isRegisterMode ? 'Реєстрація'       : 'Вхід';
    document.getElementById('authSubmitBtn').textContent= isRegisterMode ? 'Зареєструватись'  : 'Увійти';
    document.getElementById('authToggleText').textContent = isRegisterMode ? 'Вже є акаунт?'  : 'Немає акаунту?';
    document.getElementById('authToggleBtn').textContent  = isRegisterMode ? 'Увійти'          : 'Зареєструватись';
});

document.getElementById('authForm').addEventListener('submit', async (e) => {
    e.preventDefault();
    const email    = document.getElementById('authEmail').value.trim();
    const password = document.getElementById('authPassword').value;

    try {
        let url  = `${API}/login`;
        let body = { email, password };

        if (isRegisterMode) {
            url  = `${API}/register`;
            body = {
                name:           document.getElementById('regName').value.trim(),
                email, password,
                age:            Number(document.getElementById('regAge').value),
                height:         Number(document.getElementById('regHeight').value),
                weight:         Number(document.getElementById('regWeight').value),
                gender:         document.getElementById('regGender').value,
                activity_level: document.getElementById('regActivity').value,
                goal:           document.querySelector('input[name="goal"]:checked')?.value || 'maintain'
            };
        }

        const res = await fetch(url, {
            method:  'POST',
            headers: { 'Content-Type': 'application/json' },
            body:    JSON.stringify(body)
        });

        if (!res.ok) throw new Error(await res.text());

        state.user = await res.json();
        localStorage.setItem('user', JSON.stringify(state.user));

        document.getElementById('authForm').reset();
        showScreen('main');
        switchTab('profile');

    } catch (err) {
        alert('Помилка: ' + err.message);
    }
});

document.getElementById('logoutBtn').addEventListener('click', () => {
    state.user = null;
    localStorage.removeItem('user');
    showScreen('auth');
});

// ─── Таби ─────────────────────────────────────────────────────────────────────
function switchTab(tabId) {
    document.querySelectorAll('.tab-content').forEach(el => el.classList.add('hidden'));
    document.getElementById(`${tabId}-section`).classList.remove('hidden');

    if (tabId === 'profile') loadProfile();
    if (tabId === 'journal') loadJournal();
    if (tabId === 'meals')   loadMeals();
}

// ─── Профіль ──────────────────────────────────────────────────────────────────
const goalLabel = { lose: 'Схуднути', maintain: 'Підтримувати', gain: 'Набрати масу' };
const genderLabel = { male: 'Чоловік', female: 'Жінка' };

async function loadProfile() {
    if (!state.user) return;
    try {
        const res  = await fetch(`${API}/profile?user_id=${state.user.id}`);
        if (!res.ok) throw new Error(await res.text());
        const data = await res.json();

        // Оновлюємо збережені дані
        Object.assign(state.user, data);
        localStorage.setItem('user', JSON.stringify(state.user));

        document.getElementById('profInitial').textContent = data.name.charAt(0).toUpperCase();
        document.getElementById('profName').textContent    = data.name;
        document.getElementById('profAge').textContent     = data.age;
        document.getElementById('profGender').textContent  = genderLabel[data.gender] || data.gender;
        document.getElementById('profHeight').textContent  = data.height;
        document.getElementById('profWeight').textContent  = data.weight;
        document.getElementById('profGoal').textContent    = goalLabel[data.goal] || data.goal;

        const consumed   = Math.round(data.consumed_today || 0);
        const dailyNorm  = data.daily_norm || 2000;
        const remaining  = Math.max(0, dailyNorm - consumed);
        const pct        = Math.min(100, (consumed / dailyNorm) * 100);

        document.getElementById('profConsumedText').textContent = consumed;
        document.getElementById('profLimitText').textContent    = dailyNorm;
        document.getElementById('profRemainingText').textContent= remaining;

        const bar = document.getElementById('profProgressBar');
        bar.style.width = `${pct}%`;
        bar.classList.toggle('bg-red-500',    pct >= 100);
        bar.classList.toggle('bg-indigo-500', pct  < 100);

    } catch (err) {
        console.error('Профіль:', err.message);
    }
}

// ─── Щоденник ────────────────────────────────────────────────────────────────
document.getElementById('journalDate').addEventListener('change', (e) => {
    state.currentDate = e.target.value;
    loadJournal();
});

async function loadJournal() {
    if (!state.user) return;
    try {
        const res   = await fetch(`${API}/journal?user_id=${state.user.id}&date=${state.currentDate}`);
        if (!res.ok) throw new Error(await res.text());
        const items = await res.json();

        const tbody = document.getElementById('journalTableBody');
        const empty = document.getElementById('journalEmpty');
        tbody.innerHTML = '';

        if (!items || items.length === 0) {
            empty.classList.remove('hidden');
            tbody.closest('table').classList.add('hidden');
            return;
        }

        empty.classList.add('hidden');
        tbody.closest('table').classList.remove('hidden');

        for (const item of items) {
            const tr = document.createElement('tr');
            tr.className = 'border-t border-gray-700';
            tr.innerHTML = `
                <td class="p-4 text-white">${item.name}</td>
                <td class="p-4 text-gray-400">${item.weight} г</td>
                <td class="p-4 text-indigo-400 font-semibold">${Math.round(item.calories)}</td>
                <td class="p-4 text-gray-400">${item.time || '—'}</td>
            `;
            tbody.appendChild(tr);
        }
    } catch (err) {
        console.error('Щоденник:', err.message);
    }
}

// ─── Пошук продуктів (автодоповнення) ────────────────────────────────────────
function setupSearch(inputId, listId, onSelect) {
    const input = document.getElementById(inputId);
    const list  = document.getElementById(listId);
    let timer;

    input.addEventListener('input', () => {
        clearTimeout(timer);
        const q = input.value.trim().toLowerCase();
        if (q.length < 2) { list.classList.add('hidden'); return; }

        timer = setTimeout(async () => {
            try {
                const res  = await fetch(`${API}/search?q=${encodeURIComponent(q)}`);
                const data = await res.json();
                list.innerHTML = '';
                if (!data.length) { list.classList.add('hidden'); return; }
                list.classList.remove('hidden');

                for (const prod of data) {
                    const li = document.createElement('li');
                    li.className = 'px-4 py-3 hover:bg-gray-700 cursor-pointer flex justify-between text-sm';
                    li.innerHTML = `<span class="text-white">${prod.name}</span><span class="text-gray-400">${prod.calories} ккал/100г</span>`;
                    li.addEventListener('click', () => {
                        input.value = prod.name;
                        list.classList.add('hidden');
                        onSelect(prod);
                    });
                    list.appendChild(li);
                }
            } catch {}
        }, 280);
    });

    document.addEventListener('click', (e) => {
        if (!input.contains(e.target) && !list.contains(e.target))
            list.classList.add('hidden');
    });
}

// ─── Форма додавання запису в щоденник ───────────────────────────────────────
setupSearch('productSearch', 'diaryAutocompleteList', (prod) => {
    state.selectedProduct = prod;
    recalcCalories();
});

function recalcCalories() {
    if (!state.selectedProduct) return;
    const weight = Number(document.getElementById('productWeight').value) || 0;
    document.getElementById('productCalories').value =
        Math.round(state.selectedProduct.calories * weight / 100);
}

document.getElementById('productWeight').addEventListener('input', recalcCalories);

document.getElementById('addJournalEntryForm').addEventListener('submit', async (e) => {
    e.preventDefault();
    if (!state.selectedProduct) return alert('Оберіть продукт зі списку');

    const weight = Number(document.getElementById('productWeight').value);
    if (!weight || weight <= 0) return alert('Введіть коректну вагу');

    const calories = Math.round(state.selectedProduct.calories * weight / 100);
    const time     = document.getElementById('productTime').value ||
                     new Date().toTimeString().slice(0, 5);

    try {
        const res = await fetch(`${API}/journal/add`, {
            method:  'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify({
                user_id:  state.user.id,
                name:     state.selectedProduct.name,
                weight,
                calories,
                category: state.selectedProduct.category,
                time,
                date:     state.currentDate
            })
        });
        if (!res.ok) throw new Error(await res.text());

        // Скидаємо форму
        document.getElementById('productSearch').value   = '';
        document.getElementById('productWeight').value   = '100';
        document.getElementById('productCalories').value = '';
        document.getElementById('productTime').value     = '';
        state.selectedProduct = null;

        loadJournal();
        loadProfile();

    } catch (err) {
        alert('Помилка: ' + err.message);
    }
});

// ─── Мої страви ──────────────────────────────────────────────────────────────
async function loadMeals() {
    try {
        const res   = await fetch(`${API}/meals`);
        const meals = await res.json();

        const container = document.getElementById('mealsContainer');
        container.innerHTML = '';

        if (!meals.length) {
            document.getElementById('mealsEmpty').classList.remove('hidden');
            return;
        }
        document.getElementById('mealsEmpty').classList.add('hidden');

        for (const m of meals) {
            const card = document.createElement('div');
            card.className = 'bg-gray-800 border border-gray-700 rounded-xl p-5';
            card.innerHTML = `
                <h3 class="text-white font-semibold text-lg mb-3">${m.name}</h3>
                <div class="grid grid-cols-4 gap-2 text-center text-sm">
                    <div><div class="text-indigo-400 font-bold text-xl">${Math.round(m.calories)}</div><div class="text-gray-400">ккал</div></div>
                    <div><div class="text-green-400 font-bold text-xl">${m.protein}г</div><div class="text-gray-400">білки</div></div>
                    <div><div class="text-yellow-400 font-bold text-xl">${m.fats}г</div><div class="text-gray-400">жири</div></div>
                    <div><div class="text-orange-400 font-bold text-xl">${m.carbs}г</div><div class="text-gray-400">вуглев</div></div>
                </div>
            `;
            container.appendChild(card);
        }
    } catch (err) {
        console.error('Страви:', err.message);
    }
}

// ─── Конструктор страви (модальне вікно) ─────────────────────────────────────
let mealIngredients = [];

function openMealModal() {
    mealIngredients = [];
    document.getElementById('newMealName').value    = '';
    document.getElementById('mealIngredientSearch').value = '';
    updateMealTotals();
    renderMealIngredients();
    document.getElementById('createMealModal').classList.remove('hidden');
}

function closeMealModal() {
    document.getElementById('createMealModal').classList.add('hidden');
}

setupSearch('mealIngredientSearch', 'mealAutocompleteList', (prod) => {
    const grams = Number(prompt(`Скільки грамів "${prod.name}"?`, '100'));
    if (!grams || grams <= 0) return;
    mealIngredients.push({ ...prod, grams });
    document.getElementById('mealIngredientSearch').value = '';
    updateMealTotals();
    renderMealIngredients();
});

function renderMealIngredients() {
    const ul = document.getElementById('addedIngredientsList');
    ul.innerHTML = '';
    if (!mealIngredients.length) {
        ul.innerHTML = '<li class="text-gray-500 text-sm">Поки нічого не додано</li>';
        return;
    }
    for (let i = 0; i < mealIngredients.length; i++) {
        const ing = mealIngredients[i];
        const li  = document.createElement('li');
        li.className = 'flex justify-between items-center text-sm text-gray-200 py-1';
        li.innerHTML = `
            <span>${ing.name} — ${ing.grams}г</span>
            <span class="text-gray-400">${Math.round(ing.calories * ing.grams / 100)} ккал
                <button onclick="removeMealIngredient(${i})" class="ml-2 text-red-400 hover:text-red-300">✕</button>
            </span>`;
        ul.appendChild(li);
    }
}

function removeMealIngredient(i) {
    mealIngredients.splice(i, 1);
    updateMealTotals();
    renderMealIngredients();
}

function updateMealTotals() {
    const totals = mealIngredients.reduce((acc, ing) => {
        const r = ing.grams / 100;
        acc.cal  += ing.calories * r;
        acc.prot += ing.protein  * r;
        acc.fat  += ing.fats     * r;
        acc.carb += ing.carbs    * r;
        return acc;
    }, { cal: 0, prot: 0, fat: 0, carb: 0 });

    document.getElementById('mealTotalCal').textContent = Math.round(totals.cal);
    document.getElementById('mealTotalPro').textContent = totals.prot.toFixed(1);
    document.getElementById('mealTotalFat').textContent = totals.fat.toFixed(1);
    document.getElementById('mealTotalCar').textContent = totals.carb.toFixed(1);
}

async function saveMeal() {
    const name = document.getElementById('newMealName').value.trim();
    if (!name)                  return alert('Введіть назву страви');
    if (!mealIngredients.length) return alert('Додайте хоча б один інгредієнт');

    const totals = mealIngredients.reduce((acc, ing) => {
        const r = ing.grams / 100;
        acc.calories += ing.calories * r;
        acc.protein  += ing.protein  * r;
        acc.fats     += ing.fats     * r;
        acc.carbs    += ing.carbs    * r;
        return acc;
    }, { calories: 0, protein: 0, fats: 0, carbs: 0 });

    try {
        const res = await fetch(`${API}/meals`, {
            method:  'POST',
            headers: { 'Content-Type': 'application/json' },
            body:    JSON.stringify({ name, ...totals })
        });
        if (!res.ok) throw new Error(await res.text());
        closeMealModal();
        loadMeals();
    } catch (err) {
        alert('Помилка: ' + err.message);
    }
}

// ─── Закриття модального вікна "Додати продукт" (якщо використовується) ──────
function closeModal(id) {
    document.getElementById(id).classList.add('hidden');
}

// ─── Старт ────────────────────────────────────────────────────────────────────
initApp();
