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
  let serviceUuid = "0xFFE0";
  if (serviceUuid.startsWith('0x')) {
    serviceUuid = parseInt(serviceUuid);
  }


  let characteristicUuid = "0xFFE1";
  if (characteristicUuid.startsWith('0x')) {
    characteristicUuid = parseInt(characteristicUuid);
  }

  console.log('Requesting Bluetooth Device...');
  navigator.bluetooth.requestDevice(

   {filters: [
      {services: [serviceUuid]},
      //{services: ['0000ffe0-0000-1000-8000-00805f9b34fb']},
      {namePrefix: 'LOFI'},
      {namePrefix: "BT"}
    ],
    optionalServices:["0000ffe0-0000-1000-8000-00805f9b34fb"]

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

function sendData(faceData) {

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

  if (connected == true) {
  queueGattOperation(() => myCharacteristic.writeValue(data2))
  .then(() => console.log("WRITE"))
  .catch(error => console.error('Błąd podczas wysyłania danych:', error));
  }

}



let queue = Promise.resolve();

function queueGattOperation(operation) {
    queue = queue.then(operation, operation);
    return queue;
}


function handleNotifications() {
  
}