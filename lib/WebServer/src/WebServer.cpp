#include "WebServer.hpp"

#define NUM_OF_VALUE_PAIRS 5

// Html to Serve
String clientApp = "<html><head><link href=\"https://unpkg.com/material-components-web@latest/dist/material-components-web.min.css\" rel=\"stylesheet\"> <script src=\"https://unpkg.com/material-components-web@latest/dist/material-components-web.min.js\"></script> <script src=\"https://kit.fontawesome.com/64892f2c45.js\" crossorigin=\"anonymous\"></script> <link rel=\"stylesheet\" href=\"https://fonts.googleapis.com/icon?family=Material+Icons\"> <script>var errorMsg='Error reaching service, retrying in ';var seconds=30;function updateData(data){var processedData=Object.assign({temperature:null,humidity:null,heatIndex:null,celsiusOffset:null},data);document.getElementById('fahrenheit').innerText=processedData['temperature']||'N/A';document.getElementById('celsius').innerText=processedData['celsius']||'N/A';document.getElementById('humidity').innerText=processedData['humidity']||'N/A';document.getElementById('heatIndex').innerText=processedData['heatIndex']||'N/A';document.getElementById('celsiusOffset').innerText=!!processedData['celsiusOffset']||processedData['celsiusOffset']==0?processedData['celsiusOffset']:'N/A';hideLoader();} function fetchData(){var retryTimeout;var url='/data';var req=new XMLHttpRequest();req.responseType='json';req.open('GET',url,true);req.onload=function(){if(req.status==200){var jsonResponse=req.response;console.log(jsonResponse);updateData(jsonResponse);document.getElementById('results').style.display=\"block\";document.getElementById('loader').style.display=\"none\";}else{document.getElementById('loader').style.display=\"none\";var divNode=document.createElement('div');var textnode=document.createTextNode(errorMsg);var secondsNode=document.createTextNode(seconds--+' seconds!');divNode.appendChild(textnode);divNode.appendChild(secondsNode);document.body.appendChild(divNode);if(!!!retryTimeout){var interval=setInterval(()=>{secondsNode.nodeValue=seconds--+' seconds!';},1000);retryTimeout=setTimeout(()=>{seconds=30;document.body.removeChild(divNode);retryTimeout=null;clearInterval(interval);},30000);}}};req.send(null);showLoader();} function showLoader(){let elem=document.getElementById('refreshingNow');elem.innerHTML='<i class=\"fas fa-spin fa-spinner\"></i>';} function hideLoader(){let elem=document.getElementById('refreshingNow');elem.innerHTML='';} document.addEventListener('DOMContentLoaded',(event)=>{fetchData();setInterval(fetchData,30000);});</script> </head><body style=\"padding: 20px;\"><div id=\"loader\" style=\"text-align: center;\"><i class=\"fas fa-4x fa-spin fa-spinner\"></i><br /><br />Loading..., please wait!</div><div id=\"results\" style=\"display: none;\"><h1>Weather Details&nbsp;<span id=\"refreshingNow\"></span></h1><div class=\"mdc-card\"><ul class=\"mdc-list\"><li class=\"mdc-list-item\" tabindex=\"0\"> <span class=\"mdc-list-item__ripple\"></span> <span class=\"mdc-list-item__text\">Temperature:&nbsp; <span id=\"celsius\"></span>&#8451;</span>&nbsp;&nbsp;&#8594;&nbsp;&nbsp; <span id=\"fahrenheit\"></span>&#8457;</span></li><li class=\"mdc-list-item\"> <span class=\"mdc-list-item__ripple\"></span> <span class=\"mdc-list-item__text\">Celsius Offset: <span id=\"celsiusOffset\"></span></span></li><li class=\"mdc-list-item\"> <span class=\"mdc-list-item__ripple\"></span> <span class=\"mdc-list-item__text\">Humidity: <span id=\"humidity\"></span>%RH</span></li><li class=\"mdc-list-item\"> <span class=\"mdc-list-item__ripple\"></span> <span class=\"mdc-list-item__text\">Heat Index: <span id=\"heatIndex\"></span>&#8457;</span></li></ul></div></div></body></html>";

WebServer::WebServer(short port, int &offsetAddress, float &temperatureOffset) : server(new ESP8266WebServer(port)),
                                                                                 CAPACITY(JSON_OBJECT_SIZE(NUM_OF_VALUE_PAIRS)),
                                                                                 doc(DynamicJsonDocument(CAPACITY)),
                                                                                 data(doc.to<JsonObject>()),
                                                                                 offsetAddress(offsetAddress),
                                                                                 temperatureOffset(temperatureOffset)
{
    // Create jsonString to be refresh with every trigger of /data
    serializeJson(doc, jsonString);

    // Define endpoints
    defineEndPoints();

    // Add service to MDNS-SD
    MDNS.addService("http", "tcp", port);

    // Initialize EEPROM storage, we just need the size of float
    // Without this line anything reading or writing from it, wont work.
    EEPROM.begin(sizeof(float) * 4);
}

void WebServer::startMulticastDNSService(String name)
{
    if (MDNS.begin(name))
    { // Start the mDNS responder for weather.local
        Serial.println("mDNS responder started");
    }
    else
    {
        Serial.println("Error setting up MDNS responder!");
    }
}

void WebServer::handleRoot()
{
    server->send(200, "text/html", clientApp);
}

void WebServer::handleNotFound()
{
    String message = "File Not Found\n\n";
    message += "URI: ";
    message += server->uri();
    message += "\nMethod: ";
    message += (server->method() == HTTP_GET) ? "GET" : "POST";
    message += "\nArguments: ";
    message += server->args();
    message += "\n";

    for (uint8_t i = 0; i < server->args(); i++)
    {
        message += " " + server->argName(i) + ": " + server->arg(i) + "\n";
    }

    server->send(404, "text/plain", message);
}

void WebServer::defineEndPoints()
{
    server->on("/", [this]() {
        handleRoot();
    });

    // Use lambda which captures all used variables by reference (& symbol)
    server->on("/data", [this]() {
        // Clear the string
        jsonString = "";
        // Serialize the latest data to json string
        serializeJson(doc, jsonString);
        server->send(200, "application/json", jsonString);
    });
    server->onNotFound([&]() {
        handleNotFound();
    });
    server->begin();
    Serial.println("HTTP server started");

    // Offset endpoint
    server->on("/offset", HTTP_GET, [&]() {
        // Does it have the argument we listen for?
        if (server->hasArg("value"))
        {
            // Make sure to use the same address and simply replace the contents
            // Hence the use of the dereference operator to get value at address.
            temperatureOffset = server->arg("value").toFloat();
            doc["celsiusOffset"] = temperatureOffset;

            // Store it to preserve it between battery cycles
            saveValueToAddress(offsetAddress, temperatureOffset);

            // Return success
            server->send(200, "text/plain", "Got your message, dawg!!");
        }

        // Return bad request
        server->send(400, "text/plain", "We do not speak your language, please translate!!");
    });
}

void WebServer::handleClient()
{
    server->handleClient();
}

void WebServer::begin()
{
    server->begin();
}

#if IMPLEMENT_TEMPLATES_IN_CPP_FILE == true

template <typename T>
void WebServer::getValueFromAddress(int address, T &t)
{
    EEPROM.get<T>(address, t);

    Serial.print("Read value ");
    Serial.print(t);
    Serial.print(" from ");
    Serial.println(address);
}

template <typename T>
void WebServer::saveValueToAddress(int address, T &t)
{
    EEPROM.put<T>(address, t);
    EEPROM.commit();
    Serial.print("Saved value ");
    Serial.print(t);
    Serial.print(" to ");
    Serial.println(address);
}

// This line allows linking to take place, as it compile this particular
// use of the template. The idea is that a template is not code that is compiled
// until it is used. They are just declarations. This forces the definition for
// T being a float to be recognized and made ready by compiler.
// The alternative is to move the implementations above to the header file it self. In fact, if you just
// do that cut and paste below the class declaration, it will work, I tried it!!

// This also means you would have to predeclare in anticipation of all possible uses that you want. Whereas
// when declared fully in header file, all uses are allowed.
template void WebServer::getValueFromAddress<float>(int, float &);

#endif

WebServer::~WebServer()
{
    //delete jsonString;
}