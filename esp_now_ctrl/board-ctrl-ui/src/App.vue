<template>
  <div id="app">
    <h1>ESP32 Control</h1>
    <button @click="turnLEDOn">LED On</button>
    <button @click="turnLEDOff">LED Off</button>
    <div v-if="response">
      <p>Server Response: {{ response }}</p>
    </div>
  </div>
</template>

<script>
export default {
  name: 'App',
  data() {
    return {
      response: ''
    }
  },
  methods: {
    async sendCommand(command) {
      try {
        // Ensure the URL matches the route defined in ESP32 code
        const res = await fetch(`http://192.168.1.106/led/${command}`);
        const text = await res.text();
        this.response = text;
      } catch (error) {
        console.error('Error:', error);
        this.response = 'Error sending command';
      }
    },
    turnLEDOn() {
      this.sendCommand('on');
    },
    turnLEDOff() {
      this.sendCommand('off');
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
</style>
