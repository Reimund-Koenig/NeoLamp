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

    <script>
      function handle_onchange_and_reload(x) {
        document.getElementById(x).submit();
        setTimeout(function () {
          document.location.reload(false);
        }, 1000);
      }
    </script>
  </head>
  <body>
    <h2>%input_name%</h2>
    <!----------------------------------------------------->
    <!-- WakeUp Time -->
    <table>
      <tr>
        <td><hr /></td>
        <td><hr /></td>
        <td><hr /></td>
      </tr>
      <tr>
        <td class="left">Aufstehzeit:</td>
        <td class="middle">
          <form action="/get" target="hidden-form" id="getIWT">
            <input
              type="time"
              name="iWT"
              value="%iWT%"
              class="table_input"
              onchange="handle_onchange_and_reload('getIWT')"
            />
          </form>
        </td>
        <td class="right"></td>
      </tr>
      <tr>
        <td class="left">Lichtmodus:</td>
        <td class="middle">
          <form action="/get" target="hidden-form" id="getIWM">
            <select
              name="iWM"
              class="table_input"
              onchange="handle_onchange_and_reload('getIWM')"
            >
              %iWM%
            </select>
          </form>
        </td>
        <td class="right"></td>
      </tr>
      <tr %wakeup_color_row%>
        <td class="left">Farbe:</td>
        <td class="middle">
          <form action="/get" target="hidden-form" id="getIWC">
            <input
              type="color"
              name="iWC"
              value="%iWC%"
              class="table_input"
              onchange="handle_onchange_and_reload('getIWC')"
            />
          </form>
        </td>
        <td class="right"></td>
      </tr>
      <tr>
        <td class="left">Helligkeit:</td>
        <td class="middle">
          <form action="/get" target="hidden-form" id="getIWB">
            <input
              name="iWB"
              value="%iWB%"
              type="range"
              orient="vertical"
              min="0"
              max="100"
              class="table_input"
              onchange="handle_onchange_and_reload('getIWB')"
              oninput="valueIWB.innerHTML=iWB.value + ' &#0037;'"
            />
          </form>
        </td>
        <td class="right" id="valueIWB">%iWB% &#0037;</td>
      </tr>
      <tr>
        <td><hr /></td>
        <td><hr /></td>
        <td><hr /></td>
      </tr>
      <!----------------------------------------------------->
      <!-- Daytime -->
      <tr>
        <td class="left">Tags:</td>
        <td class="middle">
          <form action="/get" target="hidden-form" id="getIDT">
            <input
              type="time"
              name="iDT"
              value="%iDT%"
              class="table_input"
              onchange="handle_onchange_and_reload('getIDT')"
            />
          </form>
        </td>
        <td class="right"></td>
      </tr>
      <tr>
        <td class="left">Lichtmodus:</td>
        <td class="middle">
          <form action="/get" target="hidden-form" id="getIDM">
            <select
              name="iDM"
              class="table_input"
              onchange="handle_onchange_and_reload('getIDM')"
            >
              %iDM%
            </select>
          </form>
        </td>
        <td class="right"></td>
      </tr>
      <tr %daytime_color_row%>
        <td class="left">Farbe:</td>
        <td class="middle">
          <form action="/get" target="hidden-form" id="getIDC">
            <input
              type="color"
              name="iDC"
              value="%iDC%"
              class="table_input"
              onchange="handle_onchange_and_reload('getIDC')"
            />
          </form>
        </td>
        <td class="right"></td>
      </tr>
      <tr class="middle">
        <td class="left">Helligkeit:</td>
        <td>
          <form action="/get" target="hidden-form" id="getIDB">
            <input
              name="iDB"
              value="%iDB%"
              type="range"
              orient="vertical"
              min="0"
              max="100"
              class="table_input"
              onchange="handle_onchange_and_reload('getIDB')"
              oninput="valueIDB.innerHTML=iDB.value + ' &#0037;'"
            />
          </form>
        </td>
        <td class="right" id="valueIDB">%iDB% &#0037;</td>
      </tr>
      <tr>
        <td><hr /></td>
        <td><hr /></td>
        <td><hr /></td>
      </tr>
      <!----------------------------------------------------->
      <!-- Sleep Time  -->

      <tr>
        <td class="left">Schlafenszeit:</td>
        <td class="middle">
          <form action="/get" target="hidden-form" id="getIST">
            <input
              type="time"
              name="iST"
              value="%iST%"
              class="table_input"
              onchange="handle_onchange_and_reload('getIST')"
            />
          </form>
        </td>
        <td class="right"></td>
      </tr>
      <tr>
        <td class="left">Lichtmodus:</td>
        <td class="middle">
          <form action="/get" target="hidden-form" id="getISM">
            <select
              name="iSM"
              class="table_input"
              onchange="handle_onchange_and_reload('getISM')"
            >
              %iSM%
            </select>
          </form>
        </td>
        <td class="right"></td>
      </tr>
      <tr %sleeptime_color_row%>
        <td class="left">Farbe:</td>
        <td class="middle">
          <form action="/get" target="hidden-form" id="getISC">
            <input
              type="color"
              name="iSC"
              value="%iSC%"
              class="table_input"
              onchange="handle_onchange_and_reload('getISC')"
            />
          </form>
        </td>
        <td class="right"></td>
      </tr>
      <tr>
        <td class="left">Helligkeit:</td>
        <td class="middle">
          <form action="/get" target="hidden-form" id="getISB">
            <input
              name="iSB"
              value="%iSB%"
              type="range"
              orient="vertical"
              min="0"
              max="100"
              class="table_input"
              onchange="handle_onchange_and_reload('getISB')"
              oninput="valueISB.innerHTML=iSB.value + ' &#0037;'"
            />
          </form>
        </td>
        <td class="right" id="valueISB">%iSB% &#0037;</td>
      </tr>
      <tr>
        <td><hr /></td>
        <td><hr /></td>
        <td><hr /></td>
      </tr>
      <!----------------------------------------------------->
      <!-- Time Zone -->
      <tr>
        <td class="left">Zeitzone:</td>
        <td class="middle">
          <form action="/get" target="hidden-form" id="getTZ">
            <select
              name="input_timezone"
              class="table_input"
              onchange="handle_onchange_and_reload('getTZ')"
            >
              %input_timezone%
            </select>
          </form>
        </td>
        <td class="right"></td>
      </tr>
      <tr>
        <td><hr /></td>
        <td><hr /></td>
        <td><hr /></td>
      </tr>
      <tr>
        <td class="left">
          Uhrzeit:<br />
          <label style="font-size: 0.75em"> (der Lampe)</label>
        </td>
        <td class="middle">
          %input_time_on_load%
          <br />
          <label style="font-size: 0.75em"> (wird nicht aktualisiert)</label>
        </td>
        <td class="right"></td>
      </tr>
      <tr>
        <td><hr /></td>
        <td><hr /></td>
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
