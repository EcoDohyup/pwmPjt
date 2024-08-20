<template>
  <div id="app">
    <h1>ESP32 LED Control</h1>
    <div class="control-panel">
      <!-- LED Control -->
      <div class="led-control">
        <h2>LED Control</h2>
        <button @click="sendSignal('on')">Turn On</button>
        <button @click="sendSignal('off')">Turn Off</button>
        <p>Status: {{ status }}</p>
      </div>
    </div>
  </div>
</template>

<script>
export default {
  name: 'App',
  data() {
    return {
      status: 'Waiting for command...' // Status message to show on the UI
    }
  },
  methods: {
    async sendSignal(command) {
      try {
        const response = await fetch(`http://192.168.0.230/led/${command}`, {
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
</style>
