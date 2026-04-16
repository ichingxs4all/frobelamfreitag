const UART_SERVICE_UUID = "6e400001-b5a3-f393-e0a9-e50e24dcca9e";
const UART_TX_CHARACTERISTIC_UUID = "6e400002-b5a3-f393-e0a9-e50e24dcca9e";
const UART_RX_CHARACTERISTIC_UUID = "6e400003-b5a3-f393-e0a9-e50e24dcca9e";

var connected = false;
//var data = new Uint8Array(2);
//let myCharacteristic = null;

function connectButtonPressed() {

if (connected == true) {
}

if (connected == false) {
  connect_ble();
}

}


function connect_ble() {
  let serviceUuid = UART_SERVICE_UUID;
 


  let characteristicUuid = UART_RX_CHARACTERISTIC_UUID;


  console.log('Requesting Bluetooth Device...');
  navigator.bluetooth.requestDevice(

   {filters: [
      {services: [serviceUuid]},
      { namePrefix: "PicoUART" }
    ],
    optionalServices:[UART_SERVICE_UUID]

  }

  //{acceptAllDevices:true}
  )
  .then(device => {
    console.log(device)
    console.log('Connecting to GATT Server...');
    return device.gatt.connect();
  })
  .then(server => {
    console.log(server);
    console.log('Getting Service...');
    return server.getPrimaryService(serviceUuid);
  })
  .then(service => {
    console.log(service);
    console.log('Getting Characteristic...');
    characteristicCache = service.getCharacteristic(characteristicUuid);
    return service.getCharacteristic(characteristicUuid);
  })
  .then(characteristic => {
    console.log(characteristic);
    myCharacteristic = characteristic;
    console.log('Characteristic Properties:', myCharacteristic.properties);
// Check if .write or .writeWithoutResponse is true
    return myCharacteristic.startNotifications().then(_ => {
      console.log('> Notifications started');
      connected = true;
      myCharacteristic.addEventListener('characteristicvaluechanged',
          handleNotifications);
    });
  })
  .catch(error => {
    console.log('Argh! ' + error);
  });
}


function writeToCharacteristic(data) {



}

let isWriting = false; // Flag to prevent concurrent writes
let lastWriteTime = 0;
const WRITE_INTERVAL = 50; // Minimum ms between sends (e.g., 20Hz)

function sendData(faceData) {
  const now = Date.now();
  
  var data2 = new Uint8Array(14);
  data2[0]= 201;
  data2[1] = formatAndTrim(faceData.x);
  data2[2]= 202;
  data2[3] = formatAndTrim(faceData.y);
  data2[4]= 203;
  data2[5] = faceData.z;
  data2[6]= 204;
  data2[7] = formatAndTrim(faceData.yaw);
  data2[8] = 205;
  data2[9] = formatAndTrim(faceData.pitch);
  data2[10] = 206;
  data2[11] = faceData.roll;
  data2[12] = 207;
  data2[13] = faceData.mouthOpenness;

  if (connected && !isWriting && (now - lastWriteTime > WRITE_INTERVAL)) {
    isWriting = true;
    lastWriteTime = now;

    // Use writeValueWithoutResponse if your hardware supports it for better speed
    myCharacteristic.writeValueWithoutResponse(data2)
      .then(() => {
        isWriting = false;
        console.log("WRITE SUCCESS");
      })
      .catch(error => {
        isWriting = false;
        console.error('Error sending data:', error);
      });
  }

}



let queue = Promise.resolve();

function queueGattOperation(operation) {
    queue = queue.then(operation, operation);
    return queue;
}


function handleNotifications() {
  
}