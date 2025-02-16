#!/usr/bin/env node

// Fonction pour gérer une requête GET
function handleGetRequest() {
  const queryString = process.env.QUERY_STRING;
  console.log("Content-Type: text/plain\r\n");
  if (!queryString) {
    console.log("No query string provided.");
    return;
  }
  console.log("GET Request Received:");
  console.log("Query String: " + queryString);
}

// Fonction pour gérer une requête POST
function handlePostRequest() {
  const contentLengthStr = process.env.CONTENT_LENGTH;
  console.log("Content-Type: text/plain\r\n");
  if (!contentLengthStr) {
    console.log("No content length provided.");
    return;
  }

  const contentLength = parseInt(contentLengthStr, 10);
  if (isNaN(contentLength) || contentLength <= 0) {
    console.log("Invalid content length.");
    return;
  }

  let body = "";
  // Configure l'encodage pour recevoir du texte
  process.stdin.setEncoding("utf8");

  // Lit les données depuis l'entrée standard
  process.stdin.on("data", (chunk) => {
    body += chunk;
  });

  process.stdin.on("end", () => {
    console.log("POST Request Received:");
    console.log("Body: " + body);
  });

  // Si le flux est déjà terminé, déclenche immédiatement l'événement 'end'
  process.stdin.resume();
}

// Envoi de la ligne de statut HTTP
console.log("HTTP/1.1 200 OK");

const requestMethod = process.env.REQUEST_METHOD;
if (!requestMethod) {
  console.log("Content-Type: text/plain\r\n");
  console.log("No request method provided.");
  process.exit(1);
}

if (requestMethod === "GET") {
  handleGetRequest();
} else if (requestMethod === "POST") {
  handlePostRequest();
} else {
  console.log("Content-Type: text/plain\r\n");
  console.log("Unsupported request method: " + requestMethod);
}
