<template>
  <div id="app">
    <h1>ESP32 Motor Control</h1>
    <div class="control-panel">
      <!-- Relay Control -->
      <div class="relay-control">
        <h2>Relay Control</h2>
        <button @click="sendSignal('on')">Turn Relay On</button>
        <button @click="sendSignal('off')">Turn Relay Off</button>
        <p>Status: {{ status }}</p>
      </div>

      <!-- Motor Speed Control -->
      <div class="motor-control">
        <h2>Motor Speed Control</h2>
        <p>Current Speed: <span>{{ speed }}</span></p>
        <input type="range" min="140" max="225" step="1" v-model="speed" @input="updateSpeed">
        <input type="number" min="140" max="225" v-model="speed" @input="updateSpeed">
        <p>
          <button @click="stopMotor">Stop</button>
          <button @click="setSpeed(225)">Max Speed</button>
          <button @click="sendSpeed">Set Speed</button>
        </p>
      </div>
    </div>
  </div>
</template>

<script>
export default {
  name: 'App',
  data() {
    return {
      status: 'Waiting for command...', // Status message for relay control
      speed: 140 // Motor speed value (140-225), default to minimum
    }
  },
  methods: {
    async sendSignal(command) {
      try {
        const response = await fetch(`http://192.168.0.229/relay/${command}`, {
          method: 'GET',
          mode: 'cors',
          headers: {
            'Content-Type': 'text/plain'
          }
        });

        if (!response.ok) {
          throw new Error(`HTTP error! Status: ${response.status}`);
        }

        this.status = await response.text();
      } catch (error) {
        console.error('Error:', error);
        this.status = `Error sending signal: ${error.message}`;
      }
    },
    async sendSpeed() {
      // Ensure the speed value is within the specified range
      const clampedSpeed = Math.max(140, Math.min(225, this.speed));
      
      try {
        const response = await fetch(`http://192.168.0.229/motor/speed?value=${clampedSpeed}`, {
          method: 'GET',
          mode: 'cors',
          headers: {
            'Content-Type': 'text/plain'
          }
        });

        if (!response.ok) {
          throw new Error(`HTTP error! Status: ${response.status}`);
        }

        this.status = await response.text();
      } catch (error) {
        console.error('Error:', error);
        this.status = `Error setting speed: ${error.message}`;
      }
    },
    async stopMotor() {
      this.speed = 0;
      try {
        const response = await fetch('http://192.168.0.229/motor/speed?value=0', {
          method: 'GET',
        });
        if (!response.ok) {
          throw new Error(`HTTP error! Status: ${response.status}`);
        }
      } catch (error) {
        console.error('Error stopping motor:', error);
      }
    },
    updateSpeed() {
      // Ensure the speed value stays within the specified range
      if (this.speed == 0) this.speed == 0;
      if (this.speed < 140) this.speed = 140;
      if (this.speed > 225) this.speed = 225;
    },
    setSpeed(value) {
      // Set the speed to the specified value and update the input controls
      this.speed = value;
      this.updateSpeed();
      this.sendSpeed();
    }
  }
}
</script>

<style scoped>
#app {
  text-align: center;
  margin-top: 60px;
}

button {
  margin: 10px;
  padding: 10px;
}

p {
  margin-top: 20px;
}

input[type="range"] {
  width: 80%; /* Adjust width as needed */
}

input[type="number"] {
  margin-top: 10px;
  width: 60px;
}

h2 {
  margin-top: 20px;
}
</style>
