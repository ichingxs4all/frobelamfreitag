let vid_width = 640;
let vid_height = 480;
let ratio = vid_width / vid_height;

const container = document.getElementById('media-container');
const containerWidth = container.offsetWidth;
const containerHeight = container.offsetHeight;

if (containerWidth<containerHeight) {
  ratio = vid_height / vid_width;
}


let detections = {};

const videoElement = document.getElementById('video');
const faceMesh = new FaceMesh({
  locateFile: (file) => `https://cdn.jsdelivr.net/npm/@mediapipe/face_mesh/${file}`
});

faceMesh.setOptions({
  maxNumFaces: 1,
  minDetectionConfidence: 0.8,
  minTrackingConfidence: 0.5
});

faceMesh.onResults(gotFaces);2

function gotFaces(results) {
  detections = results;
  if (detections.multiFaceLandmarks && detections.multiFaceLandmarks.length > 0) {
    const faceData = calculateFaceData(detections.multiFaceLandmarks[0]);
    //console.log(`Face Data - Position: (${faceData.x}, ${faceData.y}, ${faceData.z}), Yaw: ${faceData.yaw}, Pitch: ${faceData.pitch}, Roll: ${faceData.roll}, Mouth Openness: ${faceData.mouthOpenness}`);
  
   let data = formatAndTrim(faceData.x)+formatAndTrim(faceData.y)+formatAndTrim(faceData.z)+formatAndTrim(faceData.yaw)+formatAndTrim(faceData.pitch)+formatAndTrim(faceData.roll)+formatAndTrim(faceData.mouthOpenness);

  sendData(faceData);
   
   updateSliderValue('x', faceData.x);
   updateSliderValue('y', faceData.y);
   updateSliderValue('z', faceData.z);
   updateSliderValue('yaw', faceData.yaw);
   updateSliderValue('pitch', faceData.pitch);
   updateSliderValue('roll', faceData.roll);
   updateSliderValue('mouthOpenness', faceData.mouthOpenness);
   

  }
}

const fps = 16; // Desired frames per second
let frameInterval = 1000 / fps; // Interval in milliseconds between frames

const camera = new Camera(videoElement, {
  onFrame: async () => {
    await new Promise(resolve => setTimeout(resolve, frameInterval)); // Wait for the frame interval to elapse
    await faceMesh.send({ image: videoElement }); // Then send the frame to faceMesh
  },
  width: vid_width,
  height: vid_height
});
camera.start();


function calculateFaceData(landmarks) {
  const noseTip = landmarks[4];
  const leftEyeOuter = landmarks[130];
  const rightEyeOuter = landmarks[359];
  const leftEyeInner = landmarks[133];
  const rightEyeInner = landmarks[362];
  const upperLipTop = landmarks[13];
  const lowerLipBottom = landmarks[14];

  // Yaw calculation (left-right)
  const yaw = Math.round(Math.atan2(rightEyeInner.x - leftEyeInner.x, rightEyeInner.z - leftEyeInner.z) * (180 / Math.PI))-40;

  // Adjusted Pitch calculation (up-down)
  let pitch = Math.round(Math.atan2((noseTip.z - (leftEyeInner.z + rightEyeInner.z) / 2), (noseTip.y - (leftEyeInner.y + rightEyeInner.y) / 2)) * (180 / Math.PI) + 90);
  pitch= pitch + 15;
  // Corrected Roll calculation (tilt)
  let roll = Math.atan2((rightEyeOuter.y - leftEyeOuter.y), (rightEyeOuter.x - leftEyeOuter.x)) * (180 / Math.PI);
  //roll = roll < 0 ? 180 + roll : roll; // Adjust roll to be within 0 to 180 degrees
  roll = Math.round((90 + roll)/1.8); // Invert to meet specification

  // Mouth Openness calculation
  const verticalDistance = Math.abs(upperLipTop.y - lowerLipBottom.y);
  const maxDistance = 0.1;
  const minDistance = 0.02;
  const normalizedDistance = (verticalDistance - minDistance) / (maxDistance - minDistance);
  const mouthOpenness = Math.round(Math.max(0, Math.min(10, normalizedDistance * 10)));

  // X, Y, Z position of the nose tip
  const x = Math.round(noseTip.x * 100);
  const y = Math.round(noseTip.y * 100);
  const z = Math.round(-noseTip.z * 100);

  return { x, y, z, yaw, pitch, roll, mouthOpenness };
}



let p5Loaded = false; // Flag to track if p5 setup is complete

let sketch = function (p) {
  let canvas;

  p.setup = function () {
    const container = document.getElementById('media-container');
    const containerWidth = container.offsetWidth;
    const containerHeight = container.offsetHeight;
    //console.log(containerWidth);
   // console.log(containerHeight);

    canvas = p.createCanvas(containerWidth, containerHeight);
    canvas.id("canvas");
    canvas.parent('media-container');   

    canvas.style('width', containerHeight*ratio + 'px');
    canvas.style('height', containerHeight + 'px');
    canvas.style('margin-left',containerWidth-containerHeight*ratio)/2 + 'px';
    canvas.style('display', 'block'); // Ensure the canvas is not inline
  

    p.colorMode(p.HSB);
    p.stroke(255);
    p.strokeWeight(3);

    p5Loaded = true; // Set the flag when setup is done
    resizeCanvas() ;
  };

  p.draw = function () {
    p.clear();
    if (detections !== undefined && detections.multiFaceLandmarks !== undefined && detections.multiFaceLandmarks.length >= 1) {
      p.faceMesh();
    }
  };

  p.faceMesh = function () {
    p.beginShape(p.POINTS);
    for (let j = 0; j < detections.multiFaceLandmarks[0].length; j++) {
      let x = detections.multiFaceLandmarks[0][j].x * p.width;
      let y = detections.multiFaceLandmarks[0][j].y * p.height;
      p.vertex(x, y);
    }
    p.endShape();
  };
};

let myp5 = new p5(sketch);


function updateSliderValue(id, value) {
  const slider = document.getElementById(id);
  const valueSpan = document.getElementById(id + '-value');
  if (slider && valueSpan) {
      slider.value = value;
      valueSpan.textContent = value;
  }
}





function formatAndTrim(value) {
  // First, ensure the value is a number and clamp it within the range of 0 to 99
  let trimmedValue = Math.max(0, Math.min(99, value));

  // Now, format the number to be always two digits
  return trimmedValue.toString().padStart(2, '0');
}






function resizeCanvas() {
  const container = document.getElementById('media-container');
  const containerWidth = container.offsetWidth;
  const containerHeight = container.offsetHeight;

  if (containerWidth<containerHeight) {
    ratio = vid_height / vid_width;
  }

    const canvasElement = document.getElementById('canvas');
    canvasElement.style.width = containerHeight*ratio + 'px';
    canvasElement.style.height = containerHeight + 'px';
    canvasElement.style.marginLeft = (containerWidth-containerHeight*ratio)/2 + 'px';
    canvasElement.style.display = 'block'; // For example, ensuring it's not inline
}

window.addEventListener('resize', resizeCanvas);


window.addEventListener('resize', () => {
  if (p5Loaded) {
    resizeCanvas();
    console.log("resize");
  }
});

document.getElementById('fullscreenBtn').addEventListener('click', function() {
  toggleFullScreen();
});

function toggleFullScreen() {
  if (!document.fullscreenElement) {
      // Request fullscreen
      document.documentElement.requestFullscreen().catch(err => {
          console.log(`Error attempting to enable full-screen mode: ${err.message} (${err.name})`);
      });
  } else {
      // Exit fullscreen
      if (document.exitFullscreen) {
          document.exitFullscreen().catch(err => {
              console.log(`Error attempting to disable full-screen mode: ${err.message} (${err.name})`);
          });
      }
  }
}

//const videoElement = document.getElementById('videoElement');

// Function to log the displayed size
function logVideoDisplayedSize() {
    const width = videoElement.clientWidth;
    const height = videoElement.clientHeight;
    console.log(`Displayed size of the video: ${width}px wide by ${height}px high.`);
}

// You can call this function at specific times or in response to events
logVideoDisplayedSize(); // Immediate check

// Also call when the window resizes, as the layout might change
window.addEventListener('resize', logVideoDisplayedSize);
