let currentExpression = "";

function appendToDisplay(value) {
    currentExpression += value;
    document.getElementById('display').textContent = currentExpression;
}

function clearDisplay() {
    currentExpression = "";
    document.getElementById('display').textContent = "";
}

function calculateResult() {
    try {
        currentExpression = eval(currentExpression).toString();
        document.getElementById('display').textContent = currentExpression;
    } catch (error) {
        document.getElementById('display').textContent = "Error";
        currentExpression = "";
    }
}
