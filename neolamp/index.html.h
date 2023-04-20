const char index_html[] PROGMEM = R"rawliteral("
<!DOCTYPE html>
<html>
  <head>
    <title>HTML Form to Input Data</title>
    <meta name="viewport" content="width=device-width, initial-scale=1" />

    <style>
      html {
        font-family: Times New Roman;
        display: inline-block;
        text-align: center;
      }
      h2 {
        font-size: 3rem;
        color: #ffaa00;
      }
    </style>

    <script>
      function handle_new_input() {
        setTimeout(function () {
          document.location.reload(false);
        }, 500);
      }
    </script>
  </head>
  <body>
    <h2>Lampe - Einstellungen</h2>
    Aktuelle Uhrzeit der Lampe: XX:XX
    <br />
    <br />
    <form action="/get" target="hidden-form">
      Schlafenszeit:
      <input type="text" name="input_sleep_time" value="%input_sleep_time%" />
      <input type="submit" value="Submit" onclick="handle_new_input()" />
    </form>
    <br />
    <form action="/get" target="hidden-form">
      Aufstehzeit:
      <input
        type="number "
        name="input_wakeup_time"
        value="%input_wakeup_time%"
      />
      <input type="submit" value="Submit" onclick="handle_new_input()" />
    </form>
    <br />
    <form action="/get" target="hidden-form">
      Animationszeit:
      <input
        type="number "
        name="input_animation_time"
        value="%input_animation_time%"
      />
      <input type="submit" value="Submit" onclick="handle_new_input()" />
    </form>
    <br />
    Zeitzone:
    <select name="timezone" id="timezone">
      <option value="berlin">Berlin</option>
      <option value="usa">USA</option>
      <option value="bla">bla</option>
      <option value="blubb">blubb</option>
    </select>
    <br />

    <br />
    Animation am Tag:
    <select name="animation" id="animation">
      <option value="mix">Farbkeise und Pulsieren</option>
      <option value="circle">Farbkeise</option>
      <option value="pulse">Pulsieren</option>
      <option value="green">Gruen</option>
      <option value="off">Lampe Ausgeschaltet</option>
    </select>
    <br />
    <iframe style="display: none" name="hidden-form"></iframe>
  </body>
</html>
)rawliteral";
