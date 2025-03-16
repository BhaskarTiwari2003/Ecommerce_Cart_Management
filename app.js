const express = require('express');
const fs = require('fs');
const cors = require('cors');

const app = express(); // ✅ Use express() instead of Router()
app.use(express.json());
app.use(cors());

let products = [
    { id: 1, name: "Laptop", price: 1000 },
    { id: 2, name: "Phone", price: 500 },
    { id: 3, name: "Headphones", price: 200 }
];

let cart = [];

// Load order history safely
const ordersFile = 'orders.json';
let orderHistory = [];

if (fs.existsSync(ordersFile)) {
    try {
        const data = fs.readFileSync(ordersFile, 'utf8').trim();
        orderHistory = data ? JSON.parse(data) : [];
    } catch (error) {
        console.error("Error reading orders.json:", error);
        orderHistory = [];
    }
} else {
    fs.writeFileSync(ordersFile, '[]'); // Ensure file exists
}

app.get('/', (req, res) => {
    res.send('Welcome to the API');
});

// Get all products
app.get('/api/products', (req, res) => {
    res.json(products);
});

// Get cart contents
app.get('/api/cart', (req, res) => {
    res.json(cart);
});

// Add item to cart
app.post('/api/cart/add/:id', (req, res) => {
    const productId = parseInt(req.params.id);
    const product = products.find(p => p.id === productId);
    if (product) {
        let cartItem = cart.find(item => item.id === productId);
        if (cartItem) {
            cartItem.quantity++;
        } else {
            cart.push({ ...product, quantity: 1 });
        }
    }
    res.json(cart);
});

// Remove item from cart
app.post('/api/cart/remove/:id', (req, res) => {
    const productId = parseInt(req.params.id);
    cart = cart.filter(item => item.id !== productId);
    res.json(cart);
});

// Checkout
app.post('/api/cart/checkout', (req, res) => {
    if (cart.length > 0) {
        orderHistory.push({ date: new Date(), items: [...cart] });
        fs.writeFileSync(ordersFile, JSON.stringify(orderHistory, null, 2));
        cart = [];
        res.json({ message: "Checkout successful!" });
    } else {
        res.json({ message: "Cart is empty." });
    }
});

// View past orders
app.get('/api/orders', (req, res) => {
    res.json(orderHistory);
});

// ✅ Export app correctly for server.js
module.exports = app;
