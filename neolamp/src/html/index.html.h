const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
  <head>
    <title>%input_name%</title>
    <meta name="viewport" content="width=device-width, initial-scale=1" />

    <style>
      html {
        display: inline-block;
        text-align: center;
      }
      h2 {
        font-size: 1.5rem;
        color: #ffaa00;
      }
      table {
        margin-left: auto;
        margin-right: auto;
      }
      td.left {
        text-align: right;
      }
      td.middle {
        text-align: left;
        width: 128px;
        padding-left: 0.5em;
      }
      td.right {
        text-align: left;
      }
      input.table_input {
        width: 128px;
      }
      select.table_input {
        width: 128px;
      }
    </style>
  </head>
  <body>
    <h2>%input_name%</h2>
    <!----------------------------------------------------->
    <!-- WakeUp Time -->
    <table>
      <tr>
        <td><hr /></td>
      </tr>
      <tr>
        <td class="middle"><a href="/settings">Einstellungen</a></td>
      </tr>
      <tr>
        <td class="middle"><a href="/settings">Einstellungen</a></td>
      </tr>
      <tr>
        <td><hr /></td>
      </tr>
    </table>
    <br />
    <br />
    <br />
    <br />
    <iframe style="display: none" name="hidden-form"></iframe>
  </body>
</html>
)rawliteral";
