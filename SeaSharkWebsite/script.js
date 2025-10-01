// Typewriter effect
function typewriter(elementId, text, speed = 70) {
  let i = 0;
  function typing() {
    if (i < text.length) {
      document.getElementById(elementId).innerHTML += text.charAt(i);
      i++;
      setTimeout(typing, speed);
    }
  }
  typing();
}

// Copy buttons
function addCopyButtons() {
  document.querySelectorAll("pre").forEach(block => {
    const btn = document.createElement("button");
    btn.className = "copy-btn";
    btn.textContent = "Copy";
    btn.onclick = () => {
      navigator.clipboard.writeText(block.innerText);
      btn.textContent = "Copied!";
      setTimeout(() => (btn.textContent = "Copy"), 2000);
    };
    block.appendChild(btn);
  });
}

document.addEventListener("DOMContentLoaded", () => {
  if (document.getElementById("typewriter-text")) {
    typewriter("typewriter-text", "Seashark: Fast, Simple, and Powerful 🦈");
  }
  addCopyButtons();
});
