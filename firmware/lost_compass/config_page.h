#pragma once

const char CONFIG_PAGE[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
<meta charset="utf-8">
<meta name="viewport" content="width=device-width,initial-scale=1">
<title>Traverse Compass — Calibration</title>
<style>
  @import url('https://fonts.googleapis.com/css2?family=IM+Fell+English&display=swap');
  *{box-sizing:border-box;margin:0;padding:0}
  body{
    background:#f2e8d0;color:#2c1a0e;
    font-family:'IM Fell English',Palatino,'Book Antiqua',serif;
    display:flex;flex-direction:column;align-items:center;
    padding:2.5rem 1.5rem;min-height:100vh;
  }
  h1{
    font-size:1rem;letter-spacing:.25em;text-transform:uppercase;
    border-bottom:1px solid #8b6914;padding-bottom:.5rem;
    margin-bottom:.3rem;width:100%;text-align:center;max-width:280px;
  }
  .sub{font-size:.72rem;letter-spacing:.18em;color:#7a5c3a;margin-bottom:2rem;font-style:italic;}
  .readout{font-size:2.2rem;letter-spacing:.04em;margin:1.2rem 0 .15rem;}
  .readout span{min-width:4ch;display:inline-block;text-align:right}
  .offset-row{font-size:.8rem;letter-spacing:.1em;color:#5a3a1a;margin-bottom:2rem;}
  .offset-row b{font-size:.95rem}
  .btn-row{display:flex;gap:.9rem;align-items:center}
  button{
    background:#8b6914;color:#f2e8d0;border:none;
    font-family:inherit;font-size:1.05rem;
    padding:.65rem 1.5rem;cursor:pointer;letter-spacing:.06em;
    box-shadow:0 2px 5px rgba(0,0,0,.35);transition:filter .1s;
  }
  button:active{filter:brightness(.78)}
  button.reset{background:#3d2005;font-size:.78rem;padding:.5rem .95rem;letter-spacing:.08em;}
  button.sm{font-size:.85rem;padding:.5rem .9rem;}
  .note{margin-top:2.5rem;font-size:.7rem;color:#9a7a50;letter-spacing:.08em;font-style:italic;text-align:center;max-width:240px;}
  .divider{width:100%;max-width:280px;border:none;border-top:1px dashed #c9a84c;margin:1.8rem 0 1.2rem;}
  .bias-row{display:flex;gap:.7rem;align-items:center;margin-bottom:.6rem;}
  .bias-lbl{font-size:.8rem;letter-spacing:.08em;min-width:5rem;color:#5a3a1a;}
  .bias-val{font-size:.95rem;min-width:4ch;display:inline-block;text-align:right;}
</style>
</head>
<body>

<h1>Magnetic Traverse</h1>
<div class="sub">Instrument Correction &#10022; Anno Domini MDCCCXXXIV</div>

<svg width="230" height="230" viewBox="-115 -115 230 230">
  <circle r="108" fill="none" stroke="#c9a84c" stroke-width="1" stroke-dasharray="2 6"/>
  <circle r="100" fill="#e8dbb0" stroke="#8b6914" stroke-width="2.5"/>
  <circle r="88" fill="none" stroke="#c9a84c" stroke-width=".8" stroke-dasharray="4 4"/>
  <g stroke="#8b6914" stroke-width="2.5">
    <line x1="0" y1="-100" x2="0" y2="-86"/>
    <line x1="0" y1="-100" x2="0" y2="-86" transform="rotate(90)"/>
    <line x1="0" y1="-100" x2="0" y2="-86" transform="rotate(180)"/>
    <line x1="0" y1="-100" x2="0" y2="-86" transform="rotate(270)"/>
  </g>
  <g stroke="#8b6914" stroke-width="1">
    <line x1="0" y1="-100" x2="0" y2="-91" transform="rotate(45)"/>
    <line x1="0" y1="-100" x2="0" y2="-91" transform="rotate(135)"/>
    <line x1="0" y1="-100" x2="0" y2="-91" transform="rotate(225)"/>
    <line x1="0" y1="-100" x2="0" y2="-91" transform="rotate(315)"/>
  </g>
  <text x="0"   y="-68" text-anchor="middle" font-size="15" font-family="'IM Fell English',Palatino,serif" fill="#8b1a1a" font-weight="bold">N</text>
  <text x="80"  y="6"   text-anchor="middle" font-size="13" font-family="'IM Fell English',Palatino,serif" fill="#2c1a0e">E</text>
  <text x="0"   y="86"  text-anchor="middle" font-size="13" font-family="'IM Fell English',Palatino,serif" fill="#2c1a0e">S</text>
  <text x="-80" y="6"   text-anchor="middle" font-size="13" font-family="'IM Fell English',Palatino,serif" fill="#2c1a0e">W</text>
  <g id="needle">
    <polygon points="0,-74 -7,0 0,6 7,0" fill="#8b1a1a"/>
    <polygon points="0,26 -7,0 0,6 7,0" fill="#2c1a0e"/>
    <circle r="6" fill="#8b6914" stroke="#2c1a0e" stroke-width="1.5"/>
    <circle r="2" fill="#2c1a0e"/>
  </g>
</svg>

<div class="readout"><span id="hdg">--</span>&deg;</div>
<div class="offset-row">Correction applied: <b><span id="off">0</span>&deg;</b></div>

<div class="btn-row">
  <button onclick="adjust(-15)">- 15&deg;</button>
  <button class="reset" onclick="resetOff()">Reset</button>
  <button onclick="adjust(+15)">+ 15&deg;</button>
</div>
<div class="btn-row" style="margin-top:.6rem">
  <button class="sm" onclick="adjust(-5)">- 5&deg;</button>
  <button class="sm" onclick="adjust(+5)">+ 5&deg;</button>
</div>

<hr class="divider">
<div class="sub" style="margin-bottom:.7rem">Dial Initialisation</div>
<p style="font-size:.7rem;color:#5a3a1a;letter-spacing:.06em;text-align:center;max-width:230px;margin-bottom:.9rem">
  Rotate the dial by hand until the needle rests on N, then confirm.
</p>
<button onclick="zeroDial()" id="zdone" style="width:100%;max-width:280px">Needle is at North &mdash; Confirm</button>
<div id="zdmsg" style="font-size:.7rem;color:#7a5c3a;font-style:italic;text-align:center;margin-top:.5rem;min-height:1em"></div>

<hr class="divider">
<div class="sub" style="margin-bottom:.9rem">Dead-Reckoning Correction</div>
<div class="bias-row">
  <span class="bias-lbl">CW Bias</span>
  <span class="bias-val" id="cwb">1.00</span>
  <button class="sm" onclick="adjBias('cw',-0.05)">-</button>
  <button class="sm" onclick="adjBias('cw',+0.05)">+</button>
</div>
<div class="bias-row">
  <span class="bias-lbl">CCW Bias</span>
  <span class="bias-val" id="ccwb">1.00</span>
  <button class="sm" onclick="adjBias('ccw',-0.05)">-</button>
  <button class="sm" onclick="adjBias('ccw',+0.05)">+</button>
</div>

<p class="note">Adjustments apply to the live compass reading and the dial in all modes.</p>

<script>
  var base = 'http://' + window.location.hostname;
  var offset = 0;

  function adjust(delta) {
    var x = new XMLHttpRequest();
    x.open('GET', base + '/setoffset?delta=' + delta, true);
    x.onload = function() { offset = parseFloat(x.responseText) || 0; showOff(); };
    x.send();
  }

  function resetOff() {
    var x = new XMLHttpRequest();
    x.open('GET', base + '/setoffset?reset=1', true);
    x.onload = function() { offset = 0; showOff(); };
    x.send();
  }

  function showOff() {
    var s = (offset > 0 ? '+' : '') + offset.toFixed(0);
    document.getElementById('off').textContent = s;
  }

  function poll() {
    var x = new XMLHttpRequest();
    x.open('GET', base + '/heading', true);
    x.onload = function() {
      try {
        var d = JSON.parse(x.responseText);
        document.getElementById('hdg').textContent = d.heading.toFixed(1);
        if (typeof d.offset !== 'undefined') { offset = d.offset; showOff(); }
        document.getElementById('needle').setAttribute('transform', 'rotate(' + d.heading + ')');
      } catch(e) {}
    };
    x.send();
  }

  function adjBias(axis, delta) {
    var x = new XMLHttpRequest();
    x.open('GET', base + '/setbias?' + axis + '=' + delta, true);
    x.onload = function() {
      var p = x.responseText.split(',');
      document.getElementById('cwb').textContent  = p[0];
      document.getElementById('ccwb').textContent = p[1];
    };
    x.send();
  }

function zeroDial() {
    var x = new XMLHttpRequest();
    x.open('GET', base + '/zerodial', true);
    x.onload = function() {
      document.getElementById('zdmsg').textContent = 'Dial synchronised.';
      setTimeout(function(){ document.getElementById('zdmsg').textContent = ''; }, 3000);
    };
    x.send();
  }

  poll();
  setInterval(poll, 1000);
</script>
</body>
</html>
)rawliteral";