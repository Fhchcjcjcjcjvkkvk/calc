let currentInput = '0';
let previousInput = '';
let operator = null;
let history = [];

function updateDisplay(value) {
  document.getElementById('display').innerText = value;
}

function appendToScreen(value) {
  if (currentInput === '0' && value !== '.') {
    currentInput = value.toString();
  } else {
    currentInput += value.toString();
  }
  updateDisplay(currentInput);
}

function clearScreen() {
  currentInput = '0';
  operator = null;
  previousInput = '';
  updateDisplay(currentInput);
}

function calculate() {
  if (operator && previousInput !== '') {
    let result;
    const prev = parseFloat(previousInput);
    const current = parseFloat(currentInput);

    switch (operator) {
      case '+':
        result = prev + current;
        break;
      case '-':
        result = prev - current;
        break;
      case '*':
        result = prev * current;
        break;
      case '/':
        result = prev / current;
        break;
      case '%':
        result = (prev * current) / 100;
        break;
      default:
        return;
    }
    currentInput = result.toString();
    updateDisplay(currentInput);
    history.push(`${previousInput} ${operator} ${currentInput} = ${result}`);
    showHistory();
    operator = null;
    previousInput = '';
  }
}

function showHistory() {
  const historyElement = document.getElementById('history');
  historyElement.innerHTML = history.map(entry => `<div>${entry}</div>`).join('');
}
