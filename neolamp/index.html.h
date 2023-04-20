const char index_html[] PROGMEM = R"rawliteral(
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
      function message_popup() {
        alert("Saved value to ESP SPIFFS");
        setTimeout(function () {
          document.location.reload(false);
        }, 500);
      }
    </script>
  </head>
  <body>
    <h2>HTML Form to Input Data</h2>
    <form action="/get" target="hidden-form">
      Enter string (current value %input_string%):
      <input type="text" name="input_string" />
      <input type="submit" value="Submit" onclick="message_popup()" />
    </form>
    <br />
    <form action="/get" target="hidden-form">
      Enter Integer (current value %input_int%):
      <input type="number " name="input_int" />
      <input type="submit" value="Submit" onclick="message_popup()" />
    </form>
    <br />
    <form action="/get" target="hidden-form">
      Enter Floating value (current value %input_float%):
      <input type="number " name="input_float" />
      <input type="submit" value="Submit" onclick="message_popup()" />
    </form>
    <iframe style="display: none" name="hidden-form"></iframe>
  </body>
</html>
)rawliteral";
