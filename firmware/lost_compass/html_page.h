// html_page.h
#pragma once

const char HTML_PAGE[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width,initial-scale=1.0">
<title>Magnetic Compass</title>
<style>
:root{--p:#efe4c4;--pd:#e2d4a4;--ink:#1c0f05;--im:#5a3820;--br:#c49a2a;--bd:#8a6b1a;--vg:#3d6448;--bo:#9a7030}
*,*::before,*::after{box-sizing:border-box;margin:0;padding:0}
body{background-color:var(--p);background-image:repeating-linear-gradient(0deg,transparent,transparent 3px,rgba(120,85,25,.03) 3px,rgba(120,85,25,.03) 4px);color:var(--ink);font-family:Palatino,'Palatino Linotype','Book Antiqua',Georgia,serif;min-height:100vh;display:flex;flex-direction:column;align-items:center;padding:18px 16px 36px}
body::before{content:'';position:fixed;inset:0;background:radial-gradient(ellipse 90% 90% at 50% 50%,transparent 40%,rgba(80,40,5,.15) 100%);pointer-events:none}
.hdr{width:100%;max-width:360px;text-align:center;border-top:2px solid var(--bo);border-bottom:2px solid var(--bo);padding:8px 4px;margin-bottom:16px;position:relative}
.hdr::before,.hdr::after{content:'';position:absolute;left:4px;right:4px;border-top:1px solid rgba(196,160,50,.4)}
.hdr::before{top:3px}.hdr::after{bottom:3px}
.hdr-t{font-size:.95rem;font-variant:small-caps;letter-spacing:.2em;color:var(--ink)}
.hdr-s{font-size:.6rem;font-style:italic;color:var(--im);letter-spacing:.1em;margin-top:2px}
.conn{position:absolute;top:50%;right:4px;transform:translateY(-50%);font-size:.58rem;letter-spacing:.06em;display:flex;align-items:center;gap:4px;color:var(--vg)}
.conn::before{content:'';width:6px;height:6px;border-radius:50%;background:var(--vg);animation:blink 2s step-end infinite}
@keyframes blink{0%,100%{opacity:1}50%{opacity:.15}}
.rose{width:192px;height:192px;margin:0 auto 14px}
.rose svg{width:100%;height:100%}
#ndl{transform-box:fill-box;transform-origin:center;transition:transform .9s cubic-bezier(.25,.46,.45,.94)}
.status{width:100%;max-width:360px;background:var(--pd);border:1px solid var(--bo);border-radius:2px;padding:14px 18px 12px;margin-bottom:12px;position:relative;box-shadow:inset 0 0 0 1px rgba(196,154,42,.25),2px 3px 8px rgba(60,30,5,.15)}
.status::before{content:'Magnetic Bearing';position:absolute;top:-8px;left:16px;font-size:.58rem;font-variant:small-caps;letter-spacing:.15em;color:var(--im);background:var(--pd);padding:0 6px}
.hdg-row{display:flex;align-items:baseline;justify-content:center;gap:4px;margin:6px 0 2px}
.hdg-v{font-size:3.4rem;color:var(--ink);line-height:1;min-width:3ch;text-align:right}
.hdg-u{font-size:1.3rem;color:var(--im);align-self:flex-start;padding-top:7px}
.hdg-c{font-size:1.4rem;color:var(--im);text-align:center;font-style:italic;letter-spacing:.15em;margin-bottom:10px}
.mode-row{display:flex;align-items:baseline;justify-content:space-between;border-top:1px solid var(--bo);padding-top:8px;margin-top:4px}
.mode-l{font-size:.58rem;font-variant:small-caps;letter-spacing:.12em;color:var(--im)}
.mode-v{font-size:.72rem;font-variant:small-caps;letter-spacing:.06em;color:var(--ink)}
.tgt-v{font-size:.62rem;font-style:italic;color:var(--bd);display:none}
.tgt-v.on{display:block}
.batt-row{display:flex;align-items:center;justify-content:space-between;border-top:1px solid var(--bo);padding-top:8px;margin-top:8px}
.batt-lbl{font-size:.58rem;font-variant:small-caps;letter-spacing:.12em;color:var(--im)}
.batt-wrap{display:flex;align-items:center;gap:7px}
.batt-v{font-size:.62rem;font-style:italic;color:var(--bd);min-width:2.6rem;text-align:right}
.btns{width:100%;max-width:360px;display:grid;grid-template-columns:1fr 1fr;gap:8px;margin-bottom:12px}
.mbtn{background:linear-gradient(160deg,#d8bc78,#b89030);border:1px solid var(--bd);color:var(--ink);font-family:inherit;font-size:.75rem;font-variant:small-caps;letter-spacing:.06em;padding:13px 8px;border-radius:2px;cursor:pointer;box-shadow:0 2px 4px rgba(60,30,5,.2),inset 0 1px 0 rgba(255,220,100,.4);transition:all .15s;text-align:center}
.mbtn:hover{background:linear-gradient(160deg,#e0c880,#c09840)}
.mbtn.active{background:linear-gradient(160deg,#4a7c5a,#2e5c3e);color:#e8f4ec;border-color:#2a5030;box-shadow:0 1px 2px rgba(20,40,20,.3),inset 0 1px 3px rgba(0,0,0,.2)}
.fp{width:100%;max-width:360px;background:var(--pd);border:1px solid var(--bo);border-radius:2px;padding:14px 18px;display:none;box-shadow:2px 3px 8px rgba(60,30,5,.15);position:relative}
.fp::before{content:'Set Fixed Bearing';position:absolute;top:-8px;left:16px;font-size:.58rem;font-variant:small-caps;letter-spacing:.15em;color:var(--im);background:var(--pd);padding:0 6px}
.fp.on{display:block}
.ig{display:flex;gap:8px;align-items:stretch;margin-bottom:10px}
.ig label{font-size:.6rem;font-variant:small-caps;letter-spacing:.08em;color:var(--im);display:flex;align-items:center;min-width:44px}
.ig input,.ig select{flex:1;background:var(--p);border:1px solid var(--bo);color:var(--ink);font-family:inherit;font-size:.85rem;padding:8px 10px;border-radius:2px;outline:none;-webkit-appearance:none;transition:border-color .15s}
.ig input:focus,.ig select:focus{border-color:var(--br)}
.ig input::placeholder{color:#a8885a;font-style:italic}
.sbtn{background:linear-gradient(160deg,#d8bc78,#b89030);border:1px solid var(--bd);color:var(--ink);font-family:inherit;font-size:.65rem;font-variant:small-caps;letter-spacing:.08em;padding:8px 12px;border-radius:2px;cursor:pointer;white-space:nowrap;transition:all .15s;box-shadow:inset 0 1px 0 rgba(255,220,100,.4)}
.sbtn:hover{background:linear-gradient(160deg,#e0c880,#c09840)}
.dv{display:flex;align-items:center;gap:10px;margin:6px 0}
.dv::before,.dv::after{content:'';flex:1;height:1px;background:var(--bo)}
.dv span{font-size:.6rem;font-style:italic;color:var(--im)}
.ft{margin-top:22px;font-size:.58rem;font-style:italic;color:var(--bd);letter-spacing:.08em;text-align:center}
.clink{color:var(--bd);text-decoration:none;border-bottom:1px solid var(--bo);letter-spacing:.08em}
.clink:hover{color:var(--br)}
</style>
</head>
<body>
<header class="hdr">
  <div class="hdr-t">Magnetic Traverse Compass</div>
  <div class="hdr-s">Field Instrument &middot; Anno Domini <span style="font-variant:small-caps">mdcccxxxiv</span></div>
  <span class="conn" id="conn">Active</span>
</header>

<div class="rose">
<svg viewBox="0 0 190 190" xmlns="http://www.w3.org/2000/svg">
  <circle cx="95" cy="95" r="87" fill="#f5edcc" stroke="#9a7030" stroke-width="1.5"/>
  <circle cx="95" cy="95" r="83" fill="none" stroke="#c4a050" stroke-width="0.5"/>
  <line x1="95" y1="8" x2="95" y2="18" stroke="#9a7030" stroke-width="0.6" transform="rotate(22.5 95 95)"/>
  <line x1="95" y1="8" x2="95" y2="18" stroke="#9a7030" stroke-width="0.6" transform="rotate(67.5 95 95)"/>
  <line x1="95" y1="8" x2="95" y2="18" stroke="#9a7030" stroke-width="0.6" transform="rotate(112.5 95 95)"/>
  <line x1="95" y1="8" x2="95" y2="18" stroke="#9a7030" stroke-width="0.6" transform="rotate(157.5 95 95)"/>
  <line x1="95" y1="8" x2="95" y2="18" stroke="#9a7030" stroke-width="0.6" transform="rotate(202.5 95 95)"/>
  <line x1="95" y1="8" x2="95" y2="18" stroke="#9a7030" stroke-width="0.6" transform="rotate(247.5 95 95)"/>
  <line x1="95" y1="8" x2="95" y2="18" stroke="#9a7030" stroke-width="0.6" transform="rotate(292.5 95 95)"/>
  <line x1="95" y1="8" x2="95" y2="18" stroke="#9a7030" stroke-width="0.6" transform="rotate(337.5 95 95)"/>
  <line x1="95" y1="8" x2="95" y2="44" stroke="#3d2010" stroke-width="1.5"/>
  <line x1="95" y1="146" x2="95" y2="182" stroke="#5a3820" stroke-width="1"/>
  <line x1="146" y1="95" x2="182" y2="95" stroke="#5a3820" stroke-width="1"/>
  <line x1="8" y1="95" x2="44" y2="95" stroke="#5a3820" stroke-width="1"/>
  <line x1="124" y1="66" x2="155" y2="35" stroke="#7a5028" stroke-width="0.8"/>
  <line x1="124" y1="124" x2="155" y2="155" stroke="#7a5028" stroke-width="0.8"/>
  <line x1="66" y1="124" x2="35" y2="155" stroke="#7a5028" stroke-width="0.8"/>
  <line x1="66" y1="66" x2="35" y2="35" stroke="#7a5028" stroke-width="0.8"/>
  <circle cx="95" cy="95" r="44" fill="#ede0b8" stroke="#9a7030" stroke-width="0.8"/>
  <circle cx="95" cy="95" r="40" fill="none" stroke="#c4a050" stroke-width="0.4"/>
  <text x="95" y="30" text-anchor="middle" fill="#1c0f05" font-family="Palatino,'Palatino Linotype',Georgia,serif" font-size="15" font-style="italic" font-weight="bold">N</text>
  <text x="95" y="174" text-anchor="middle" fill="#5a3820" font-family="Palatino,'Palatino Linotype',Georgia,serif" font-size="12" font-style="italic">S</text>
  <text x="174" y="99" text-anchor="middle" fill="#5a3820" font-family="Palatino,'Palatino Linotype',Georgia,serif" font-size="12" font-style="italic">E</text>
  <text x="16" y="99" text-anchor="middle" fill="#5a3820" font-family="Palatino,'Palatino Linotype',Georgia,serif" font-size="12" font-style="italic">W</text>
  <g id="ndl">
    <path d="M95,27 L91.5,90 L95,95 L98.5,90 Z" fill="#1c0f05"/>
    <path d="M95,163 L91.5,100 L95,95 L98.5,100 Z" fill="#c49a2a"/>
    <circle cx="95" cy="95" r="5.5" fill="#c49a2a" stroke="#8a6b1a" stroke-width="1"/>
    <circle cx="95" cy="95" r="2" fill="#1c0f05"/>
  </g>
</svg>
</div>

<div class="status">
  <div class="hdg-row">
    <span class="hdg-v" id="hv">000</span>
    <span class="hdg-u">&deg;</span>
  </div>
  <div class="hdg-c" id="hc">N</div>
  <div class="mode-row">
    <span class="mode-l">Comportment</span>
    <span class="mode-v" id="mv">Free Needle</span>
    <span class="tgt-v" id="tv">Course: &mdash;</span>
  </div>
  <div class="batt-row">
    <span class="batt-lbl">Voltaic Reserve</span>
    <div class="batt-wrap">
      <svg id="bsvg" width="70" height="20" viewBox="0 0 70 20">
        <rect x="1" y="2" width="61" height="16" rx="2" fill="none" stroke="#9a7030" stroke-width="1.5"/>
        <rect x="62" y="7" width="6" height="6" rx="1" fill="#9a7030"/>
        <rect class="bs" x="3"  y="4" width="10" height="12" rx="1"/>
        <rect class="bs" x="14" y="4" width="10" height="12" rx="1"/>
        <rect class="bs" x="25" y="4" width="10" height="12" rx="1"/>
        <rect class="bs" x="36" y="4" width="10" height="12" rx="1"/>
        <rect class="bs" x="47" y="4" width="10" height="12" rx="1"/>
      </svg>
      <span class="batt-v" id="bv">&mdash;</span>
    </div>
  </div>
</div>

<div class="btns">
  <button class="mbtn" id="btn-normal"      onclick="setMode('normal')">Free Needle</button>
  <button class="mbtn" id="btn-lazyspin"    onclick="setMode('lazyspin')">Lazy Revolution</button>
  <button class="mbtn" id="btn-erraticspin" onclick="setMode('erraticspin')">Erratic Motion</button>
  <button class="mbtn" id="btn-fixed"       onclick="setMode('fixed')">Fixed Bearing</button>
</div>

<div class="fp" id="fp">
  <div class="ig">
    <label>Bearing</label>
    <input type="number" id="di" min="0" max="359" placeholder="0 &ndash; 359&deg;">
    <button class="sbtn" onclick="setFD()">Set</button>
  </div>
  <div class="dv"><span>or</span></div>
  <div class="ig">
    <label>Point</label>
    <select id="cs">
      <option value="0">N &mdash; 0&deg;</option>
      <option value="45">NE &mdash; 45&deg;</option>
      <option value="90">E &mdash; 90&deg;</option>
      <option value="135">SE &mdash; 135&deg;</option>
      <option value="180">S &mdash; 180&deg;</option>
      <option value="225">SW &mdash; 225&deg;</option>
      <option value="270">W &mdash; 270&deg;</option>
      <option value="315">NW &mdash; 315&deg;</option>
    </select>
    <button class="sbtn" onclick="setFC()">Set</button>
  </div>
</div>

<div class="ft">ESP32C3 &middot; 192.168.4.1 &middot; Observed each second &middot; <a href="/config" class="clink">Instrument Correction</a></div>

<script>
var MODES=['normal','lazyspin','erraticspin','fixed'];
var DIRS=['N','NNE','NE','ENE','E','ESE','SE','SSE','S','SSW','SW','WSW','W','WNW','NW','NNW'];
var MLBL={normal:'Free Needle',lazyspin:'Lazy Revolution',erraticspin:'Erratic Motion',fixed:'Fixed Bearing'};
var BASE='http://'+window.location.hostname;

var BCOL=['','#7a2015','#8a4515','#9a7030','#c49a2a','#c49a2a'];

function d2c(d){return DIRS[Math.round(((d%360)+360)%360/22.5)%16];}
function pad(d){return String(Math.round(d)%360).padStart(3,'0');}

function get(url,cb){var x=new XMLHttpRequest();x.open('GET',url,true);x.timeout=800;x.onload=function(){if(x.status===200&&cb)cb(x.responseText);};x.send();}

function updBatt(bars,voltage){
  var segs=document.querySelectorAll('.bs');
  var col=bars>0?BCOL[bars]:'none';
  segs.forEach(function(s,i){
    if(i<bars){s.setAttribute('fill',col);s.removeAttribute('stroke');}
    else{s.setAttribute('fill','none');s.setAttribute('stroke','#9a7030');s.setAttribute('stroke-width','0.5');}
  });
  document.getElementById('bv').textContent=voltage!=null?voltage.toFixed(2)+'V':'\u2014';
}

function upd(data){
  document.getElementById('hv').textContent=pad(data.heading);
  document.getElementById('hc').textContent=d2c(data.heading);
  document.getElementById('ndl').style.transform='rotate('+data.heading+'deg)';
  document.getElementById('mv').textContent=MLBL[data.mode]||data.mode;
  var tv=document.getElementById('tv');
  if(data.mode==='fixed'){tv.textContent='Course: '+pad(data.target)+'\u00b0 '+d2c(data.target);tv.classList.add('on');}
  else{tv.classList.remove('on');}
  MODES.forEach(function(m){var b=document.getElementById('btn-'+m);if(b)b.classList.toggle('active',m===data.mode);});
  document.getElementById('fp').classList.toggle('on',data.mode==='fixed');
  if(data.bb!=null)updBatt(data.bb,data.bv);
}

function poll(){
  get(BASE+'/heading',function(txt){
    try{
      var d=JSON.parse(txt);
      var c=document.getElementById('conn');c.textContent='Active';c.style.color='#3d6448';
      upd(d);
    }catch(e){}
  });
}
function setMode(m){
  MODES.forEach(function(x){var b=document.getElementById('btn-'+x);if(b)b.classList.toggle('active',x===m);});
  document.getElementById('fp').classList.toggle('on',m==='fixed');
  get(BASE+'/set?mode='+m,null);
}
function setFD(){
  var v=parseInt(document.getElementById('di').value,10);
  if(isNaN(v)||v<0||v>359){document.getElementById('di').style.borderColor='#7a2015';setTimeout(function(){document.getElementById('di').style.borderColor='';},1000);return;}
  get(BASE+'/set?mode=fixed&deg='+v,null);
}
function setFC(){get(BASE+'/set?mode=fixed&deg='+parseFloat(document.getElementById('cs').value),null);}
poll();
setInterval(poll,1000);
</script>
</body>
</html>
)rawliteral";