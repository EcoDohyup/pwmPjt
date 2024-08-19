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
        <input type="range" min="0" max="255" v-model="speed" @input="updateSpeed">
        <input type="number" v-model="speed" @input="updateSpeed" min="0" max="255">
        <p>
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
      speed: 0 // Motor speed value (0-255)
    }
  },
  methods: {
    async sendSignal(command) {
      try {
        const response = await fetch(`http://192.168.0.230/relay/${command}`, {
          method: 'GET'
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
      try {
        const response = await fetch(`http://192.168.0.230/motor/speed/${this.speed}`, {
          method: 'GET'
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
    updateSpeed() {
      this.status = `Speed set to ${this.speed}`;
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
  width: 100%;
}

input[type="number"] {
  margin-top: 10px;
  width: 60px;
}

h2 {
  margin-top: 20px;
}
</style>
