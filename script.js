function appendToDisplay(value) {
  document.getElementById("display").value += value;
}

function clearDisplay() {
  document.getElementById("display").value = '';
}

function calculate() {
  try {
    let result = eval(document.getElementById("display").value);
    document.getElementById("display").value = result;
  } catch (error) {
    document.getElementById("display").value = 'Error';
  }
}

function executeCommand() {
  const command = document.getElementById("command-input").value.split(' ');
  const ticker = command[0];
  const option = command[1];

  fetch(`https://query1.finance.yahoo.com/v7/finance/quote?symbols=${ticker}`)
    .then(response => response.json())
    .then(data => {
      let value;
      switch (option) {
        case 'max':
          value = data.quoteResponse.result[0].regularMarketDayHigh;
          break;
        case 'min':
          value = data.quoteResponse.result[0].regularMarketDayLow;
          break;
        default:
          value = "Invalid Option";
      }
      alert(`The ${option} of ${ticker} is: ${value}`);
    })
    .catch(error => alert("Failed to fetch data"));
}

function magicFunction(type) {
  let result;
  switch (type) {
    case 'time':
      result = new Date().toLocaleTimeString();
      break;
    case 'temperature':
      result = prompt("Enter temperature in Celsius: ");
      result = (result * 9/5) + 32 + " °F"; // Convert to Fahrenheit
      break;
    case 'speed':
      let distance = prompt("Enter distance (km): ");
      let time = prompt("Enter time (hours): ");
      result = (distance / time) + " km/h";
      break;
    case 'data':
      result = prompt("Enter data size (MB): ");
      result = result * 1024 + " KB"; // Convert MB to KB
      break;
    case 'area':
      let length = prompt("Enter length (m): ");
      let width = prompt("Enter width (m): ");
      result = length * width + " m²"; // Area
      break;
    case 'tip':
      let bill = prompt("Enter bill amount: ");
      let percentage = prompt("Enter tip percentage: ");
      result = (bill * (percentage / 100)) + " Tip";
      break;
  }
  alert(`Magic Result: ${result}`);
}
