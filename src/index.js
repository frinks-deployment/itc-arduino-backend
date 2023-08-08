import net from 'net';
import express from 'express';
import { config } from "dotenv";
import bodyParser from 'body-parser';
import axios from 'axios';

config();

// Create an Express app
const app = express();

app.use(bodyParser.json({ limit: '50mb' }));
app.use(bodyParser.urlencoded({ limit: '50mb', extended: true, parameterLimit: 50000 }));

// Start the Express server
app.listen(process.env.EXPRESS_PORT, () => {
  console.log(`Express server listening on port ${process.env.EXPRESS_PORT}`);
});

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
    axios.put('/api/transaction/arduino-ping', {});
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