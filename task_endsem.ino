#include "WiFi.h" //including required libraries.
#include <HTTPClient.h>
#include <stdio.h>

// WiFi credentials
const char* ssid = "qqqqqqqq";  // My Mobile SSID.
const char* password = "qqqqqqqq";  // Hotspot password.

// declaring a server
WiFiServer server(80);

// Google script ID and required credentials.
String GOOGLE_SCRIPT_ID = "AKfycbw6W8C6JUymyrlu0thOSpqb5lbadJzGuQhCxDaZaFozmW28Ldk1ctp-mqoFB3-Zhus9";

//declring some variables required globally.
int var;
int var1;
int with_amt;
int dep_amt;
int tag = 0; // initializing a tag variable usefulin upcoming code for updating balance into an array.
int total[100] = {15000}; // array where balance is stored after ecah transaction.

// Function for withdrawal calculations.
int withdrawal(int w,int tag)
{
    int bal;
    int a = sizeof(total)/sizeof(total[0]);
    bal = total[tag]-w;
    total[tag+1]=bal;
    return bal;
}

//Function for deposit calculations.
int deposit(int d,int tag)
{
  int bal;
    int a = sizeof(total)/sizeof(total[0]);
    bal = total[tag]+d;
    total[tag+1]=bal;
    return bal;
}

//Setup function (executed once).
void setup () {
  delay (1000);
  Serial.begin(115200);
  delay(1000);
  // connect to WiFi
  Serial.println ();
  Serial.print("Connecting to wifi: ");
  Serial.println(ssid);
  Serial.flush() ;
  WiFi.begin (ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay (500);
    Serial.print("."); // if not connected
  }
  pinMode(2,OUTPUT);
}

// Loop function (executes repeatedly).
void loop () {
  if (WiFi.status () == WL_CONNECTED) {
    char a = 0;
    int b = 0;
    String list[100]; // this array is to tore the nput given from google sheets.
    int count = 0;
    String variable ;
    HTTPClient http;
    a = Serial.read();
    String url = "https://script.google.com/macros/s/" + GOOGLE_SCRIPT_ID + "/exec?read";
    Serial.println("Making a request"); // this message on serial monitor shows that process of fetching data is started.
    http.begin(url.c_str()); 
    http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
    int httpCode = http.GET();
    String dataFromSheet;
    if (httpCode > 0) { 
      dataFromSheet = http.getString(); // string fetched from google sheet is stored in a variable.
      Serial.println(httpCode);
      Serial.println(dataFromSheet);

      // This while loop is for splitting the string to get separate data.      
      while(dataFromSheet.length() > 0)
      {
        b = dataFromSheet.indexOf(' ');
        if(b == -1)
        {
          list[count++]=dataFromSheet;
          break;
        }
        else 
        {
          list[count++]=dataFromSheet.substring(0,b);
          dataFromSheet = dataFromSheet.substring(b+1);
        }
      }
      var = list[2].toInt(); // string to integer convertion for withdrawal data.
      var1 = list[3].toInt(); // string to integer convertion for deposit data.
      if(list[0]=="Rohitha")
      {
        Serial.println("User Name is verified"); // verification of username.
        if(list[1]=="Jhansi")
        {
          Serial.println("Password is verified"); // verification of password.
          Serial.println("Total Balance available is Rs.15000"); // Initial balance printing.
          with_amt = withdrawal(var,tag);
          Serial.println("Withdrawn Amount :"); // Withdrawn amount in this transaction.
          Serial.println(var);
          dep_amt = deposit(var1,tag);
          Serial.println("Deposited Amount :"); // Deposited amount in this transaction.
          Serial.println(var1);
          if(var != 0)
          {
            Serial.println("Balance after withdrawal :"); // Balance after withdrawal.
            Serial.println(with_amt);
          }
          if(var1 != 0)
          {
            Serial.println("Balance after deposit :"); // Balance after deposit.
            Serial.println(dep_amt);
          }
          Serial.println("IP address: "); 
          Serial.println(WiFi.localIP()); // IP address of the webpage created is displayed.
          server.begin();
        }
        else
        {
          Serial.println("Password is incorrect");
        }
      }
      else
      {
        Serial.println("Username is incorrect");
      }
    }
    else {
      Serial.println("Error on HTTP request"); // for error in request.
    }
    // http.end();
  }
  
  // webpage code.
  WiFiClient client = server.available();
  if(client)
  {
    String request = client.readStringUntil('\r');
    client.println("HTTP/1.1 200 OK");
    client.println("Content-type:text/html");
    client.println("Connection:close");
    client.println();
    // start of html code.
    client.println("<!DOCTYPE html><html>");
    // setting page size to fit any device's screen.
    client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
    client.println("<link rel\"icon\" href=\"data:,\">");

    // writing css for styling the web page created.
    client.println("<style>body {text-align: center; font-family: Garamond, serif;font-size: x-large;}");
    client.println("table { border-collapse: collapse; width:40%; margin-left:auto; margin-right:auto;border-spacing: 2px;background-color: white; border: 4px solid blue; }");
    client.println("th { padding: 20px; background-color: #0000FF; color: white}");
    client.println("tr { border: 5px solid blue; padding: 2px; }");
    client.println("tr: hover {background-color:yellow; }");
    client.println("td { border: 4px; padding: 12px; }");
    client.println(".amount {color:black; font-weight: bold; background-color: white; padding: 1px; }");

    // writing body of html.
    client.println("</style></head></body><h1>ESP32 Web Server</h1>");
    client.println("<h2>ATM Machine</h2>");
    client.println("<table><tr><th>CATEGORY</th><th>AMOUNT</th></tr>"); // creating a table for displaying
    client.println("<tr><td><b>Balance Now</b></td><td><span class=\"amount\">"); // writing in table.
    client.println(with_amt);
    client.println("<tr><td><b>Amount withdrawn </b></td><td><span class=\"amount\">");
    client.println(var);
    client.println("<tr><td><b>Amount deposited</b></td><td><span class=\"amount\">");
    client.println(var1);
    client.println(" </span></td></tr>");
    client.println("</body></html>"); // end of the html page.
    Serial.println("Client disconnected.");
  }
  tag++; // incrementing the tag.
  delay (1000); // sufficient delay.
}
