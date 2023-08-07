import net from 'net';
import express from 'express';
import { config } from "dotenv";
import bodyParser from 'body-parser';

config();

// Create a TCP server
const server = net.createServer();

// Track connected clients
const clients = [];

// Handle new client connections
server.on('connection', socket => {
  // Add the client socket to the list
  console.log('Arduino connected:', socket.remoteAddress, socket.remotePort);
  clients.push(socket);

  // Handle incoming data from the client
  socket.on('data', data => {
    console.log(`Received data from client: ${data}`);
  });

  // Handle client disconnection
  socket.on('end', () => {
    // Remove the client socket from the list
    const index = clients.indexOf(socket);
    if (index !== -1) {
      clients.splice(index, 1);
    }
    console.log('Client disconnected');
  });

  // Handle socket errors
  socket.on('error', error => {
    console.error('Socket error:', error);
  });
});

// Start the server
server.listen(process.env.TCP_PORT, () => {
  console.log(`Server listening on port ${process.env.TCP_PORT}`);
});

// Function to broadcast data to all connected clients
const broadcastData = async (data) => {
  console.log("SENDING DATA--", data);
  clients.forEach(client => {
    client.write(data);
  });
}

// Create an Express app
const app = express();

app.use(bodyParser.json({ limit: '50mb' }));
app.use(bodyParser.urlencoded({ limit: '50mb', extended: true, parameterLimit: 50000 }));

app.post('/increment', async (req, res) => {
  // const belt_id='WLM-1,663BC1', licence_number="HR12345", loading_count=20, bag_limit=5000;
  const { belt_id, licence_number, loading_count, bag_limit } = req.body;
  const length = `${loading_count}`.length;
  const data = `b:${belt_id.toLowerCase()}/c:${'0'.repeat(4 - length)}${loading_count}/l:${bag_limit}/n:${licence_number.toLowerCase()}/.`;
  await broadcastData(data);
  res.send("OK");
});

app.post('/clear', async (req, res) => {
  // const belt_id='TLM-1';
  const { belt_id } = req.body;
  const data = `b:${belt_id.toLowerCase()}/clear.`;
  await broadcastData(data);
  res.send("OK");
});

// Start the Express server
app.listen(process.env.EXPRESS_PORT, () => {
  console.log(`Express server listening on port ${process.env.EXPRESS_PORT}`);
});