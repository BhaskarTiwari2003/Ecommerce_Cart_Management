const http = require('http');
const app = require('./app'); // ✅ Correct import

const PORT = 3000;
const server = http.createServer(app);

server.listen(PORT, () => {
    console.log(`Server running on http://localhost:${PORT}`);
});
