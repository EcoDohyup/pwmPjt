<template>
  <div id="app">
    <h1>ESP32 Motor Control</h1>
    <div class="control-panel">
      <!-- Relay Control -->
      <div class="relay-control">
        <h2>Relay Control</h2>
        <button @click="sendRelayCommand('on')">Relay On</button>
        <button @click="sendRelayCommand('off')">Relay Off</button>
        <p>Relay Status: {{ relayStatus }}</p>
      </div>

      <!-- Motor Speed Control -->
      <div class="motor-control">
        <h2>Motor Speed Control</h2>
        <input type="number" v-model.number="motorSpeed" min="0" max="255" />
        <button @click="setMotorSpeed">Set Speed</button>
        <p>Current Speed: {{ currentSpeed }}</p>
      </div>
    </div>
  </div>
</template>

<script>
export default {
  name: 'App',
  data() {
    return {
      relayStatus: 'OFF',  // Relay status display
      motorSpeed: 0,       // Speed input model
      currentSpeed: 0      // Current speed display
    }
  },
  methods: {
    async sendRelayCommand(command) {
      try {
        await fetch(`http://192.168.1.106/relay/${command}`);
        this.relayStatus = command.toUpperCase(); // Update relay status
      } catch (error) {
        console.error('Error:', error);
        this.relayStatus = 'Error';
      }
    },
    async setMotorSpeed() {
      try {
        const res = await fetch(`http://192.168.1.106/setSpeed?value=${this.motorSpeed}`);
        const speed = await res.text();
        this.currentSpeed = speed; // Update current speed display
      } catch (error) {
        console.error('Error:', error);
        this.currentSpeed = 'Error';
      }
    }
  }
}
</script>

<style scoped>
#app {
  text-align: center;
  margin-top: 60px;
}

.control-panel {
  display: flex;
  justify-content: space-around;
  margin: 20px;
}

.relay-control, .motor-control {
  margin: 10px;
}

button {
  margin: 5px;
  padding: 10px;
}

input {
  margin: 10px;
  padding: 5px;
}
</style>
