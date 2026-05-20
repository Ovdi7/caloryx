const API_BASE = 'http://localhost:8080/api';
let isLoginMode = true;

// DOM Elements
const authForm = document.getElementById('authForm');
const authToggleBtn = document.getElementById('authToggleBtn');
const dateInput = document.getElementById('journalDate');
const productModal = document.getElementById('productModal');
const addProductForm = document.getElementById('addProductForm');
const alertModal = document.getElementById('alertModal');

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
    const sections = ['profile', 'journal', 'stats', 'about'];

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
    }
}

// Auth Logic
authToggleBtn.addEventListener('click', () => {
    isLoginMode = !isLoginMode;
    document.getElementById('authTitle').innerText = isLoginMode ? 'Вхід' : 'Реєстрація';
    document.getElementById('authSubmitBtn').innerText = isLoginMode ? 'Увійти' : 'Зареєструватись';
    document.getElementById('authToggleText').innerText = isLoginMode ? 'Немає акаунту?' : 'Вже є акаунт?';
    authToggleBtn.innerText = isLoginMode ? 'Зареєструватись' : 'Увійти';
    document.getElementById('registerFields').classList.toggle('hidden', isLoginMode);
});

authForm.addEventListener('submit', async (e) => {
    e.preventDefault();
    
    const email = document.getElementById('authEmail').value;
    const password = document.getElementById('authPassword').value;
    
    const url = `${API_BASE}/${isLoginMode ? 'login' : 'register'}`;
    const payload = { email, password };
    
    if (!isLoginMode) {
        payload.name = document.getElementById('regName').value;
        payload.gender = document.getElementById('regGender').value;
        payload.age = parseInt(document.getElementById('regAge').value) || 0;
        payload.height = parseFloat(document.getElementById('regHeight').value) || 0;
        payload.weight = parseFloat(document.getElementById('regWeight').value) || 0;
        payload.activity_level = document.getElementById('regActivity').value;
        payload.goal = document.querySelector('input[name="goal"]:checked')?.value || 'maintain';
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

// Load Profile
async function loadProfile() {
    const userId = localStorage.getItem('userId');
    if (!userId) return switchTab('auth');

    try {
        const res = await fetch(`${API_BASE}/profile?user_id=${userId}`);
        if (!res.ok) throw new Error("Не вдалося завантажити профіль");
        
        const data = await res.json();
        
        document.getElementById('profInitial').innerText = (data.name || 'U').charAt(0).toUpperCase();
        document.getElementById('profAge').innerText = data.age || '-';
        document.getElementById('profGender').innerText = data.gender === 'male' ? 'Чоловік' : 'Жінка';
        document.getElementById('profHeight').innerText = data.height || '-';
        document.getElementById('profWeight').innerText = data.weight || '-';
        
        const goalsMap = { 'lose': 'Схуднення', 'maintain': 'Підтримка', 'gain': 'Набір маси' };
        document.getElementById('profGoal').innerText = goalsMap[data.goal] || '-';

        const limit = data.dailyLimit || 2200; 
        const consumed = data.consumedToday || 0;
        
        document.getElementById('profLimitText').innerText = limit;
        document.getElementById('profConsumedText').innerText = consumed;
        
        const percent = Math.min((consumed / limit) * 100, 100);
        document.getElementById('profProgressBar').style.width = `${percent}%`;
        
    } catch (err) {
        console.warn('API /profile не реалізовано або віддає помилку:', err.message);
    }
}

// Load Journal
async function loadJournal() {
    const userId = localStorage.getItem('userId');
    if (!userId) return;
    
    const date = dateInput.value;
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

// Add Product API Post
document.getElementById('addProductForm').addEventListener('submit', async (e) => {
    e.preventDefault();
    const userId = localStorage.getItem('userId');
    
    const payload = {
        user_id: parseInt(userId),
        name: document.getElementById('prodName').value,
        weight: parseFloat(document.getElementById('prodWeight').value),
        calories: parseFloat(document.getElementById('prodCal').value),
        category: document.getElementById('prodCat').value,
        time: document.getElementById('prodTime').value,
        date: dateInput.value
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
            openAlert('Продукт успішно додано!', false);
            loadJournal();
        } else {
            throw new Error();
        }
    } catch (err) {
        openAlert('Помилка додавання продукту', true);
    }
});

// Modal Interactions
function openProductModal() {
    productModal.classList.remove('hidden');
    document.getElementById('prodTime').value = new Date().toTimeString().slice(0, 5);
}

function closeProductModal() {
    productModal.classList.add('hidden');
}

function openAlert(msg, isError = false) {
    const box = document.getElementById('alertBox');
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