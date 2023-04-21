echo 'const char index_html[] PROGMEM = R"rawliteral("' > tmp_index.html.h
cat index.html >> tmp_index.html.h
echo ')rawliteral";' >> tmp_index.html.h
mv tmp_index.html.h ./neolamp/src/constants/index.html.h