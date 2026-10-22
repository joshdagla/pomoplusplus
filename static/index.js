const startBtn = document.querySelector("#startbtn");
const stopBtn = document.querySelector("#pausebtn");
const startSound = new Audio("/static/sounds/pomo_timer.mp3");
const endSound = new Audio("/static/sounds/timer_end.mp3");
const resetBtn = document.querySelector("#resetbtn");
const progressbar = document.querySelector(".progressbar");
const progressbarNumber = document.querySelector(
  ".progressbar .progressbar-number",
);
const pomodoroBtn = document.getElementById("pomodorobtn");
const shortbrkBtn = document.getElementById("shortbrkbtn");
const longbrkBtn = document.getElementById("longbrkbtn");
const pomCount = document.querySelector(".pomdoro-count");

let pomdoroCount = 0;
const pomodorountilLongbrk = 4;
const pomodorotimer = 1500; /* 25 minutes*/
const shortbreaktimer = 300; /* 5 minutes*/
const longbreaktimer = 900; /* 15 minutes*/
let timerValue = pomodorotimer;
let multipliervalue = 360 / timerValue;
let progressInterval;
let pomodoroType = "POMODORO";

startBtn.addEventListener("click", startTimer);
stopBtn.addEventListener("click", pauseTimer);
resetBtn.addEventListener("click", resetTimer);
pomodoroBtn.addEventListener("click", () => setTimeType("POMODORO"));
shortbrkBtn.addEventListener("click", () => setTimeType("SHORTBREAK"));
longbrkBtn.addEventListener("click", () => setTimeType("LONGBREAK"));

function startTimer() {
  clearInterval(progressInterval); // prevents stacked intervals
  startSound.play();
  progressInterval = setInterval(() => {
    timerValue--;
    setProgressInfo();
    if (timerValue === 0) {
      endSound.play();
      clearInterval(progressInterval);
      pomdoroCount++;
      pomCount.style.display = "block";
      pomCount.style.color = "#3b2a20";
      pomCount.style.fontSize = "30px";
      pomCount.textContent = `Pomodoro Count ${pomdoroCount}`;
      if (pomdoroCount % pomodorountilLongbrk === 0) {
        longbrkBtn.style.display = "flex";
      }
      setTimeType(pomodoroType);
    }
  }, 1000);
}

function setProgressInfo() {
  progressbarNumber.textContent = NumbertoString(timerValue);
  progressbar.style.background = `conic-gradient(#d94530 ${
    timerValue * multipliervalue
  }deg, #e8cdb4 0deg)`;
}

function NumbertoString(number) {
  const minutes = Math.trunc(number / 60)
    .toString()
    .padStart(2, "0");
  const seconds = Math.trunc(number % 60)
    .toString()
    .padStart(2, "0");
  return `${minutes}:${seconds}`;
}

function pauseTimer() {
  clearInterval(progressInterval);
}

function setTimeType(type) {
  pomodoroType = type;
  pomodoroBtn.classList.toggle("active", type === "POMODORO");
  shortbrkBtn.classList.toggle("active", type === "SHORTBREAK");
  longbrkBtn.classList.toggle("active", type === "LONGBREAK");
  resetTimer();
}

function resetTimer() {
  clearInterval(progressInterval);
  timerValue =
    pomodoroType === "POMODORO"
      ? pomodorotimer
      : pomodoroType === "SHORTBREAK"
        ? shortbreaktimer
        : longbreaktimer;
  multipliervalue = 360 / timerValue;
  setProgressInfo();
}
