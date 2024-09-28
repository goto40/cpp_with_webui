<script lang="ts">
	let someNumber = $state(10);
	let someResult = $state<number | string>('(no result so far)');
	let port = 18080;
	let servedFromCpp = window.location.href.includes(`:${port}/`);
	let host = (() => {
		if (servedFromCpp) {
			return window.location.href.match(/http(?:s?):\/\/([^:/]+):(\d+)?/)[1];
		} else {
			return 'localhost';
		}
	})();

	let logs = $state('');
	let logsDiv = $state<HTMLDivElement | undefined>(undefined);

	let createSocket = () => {
		const ws = new WebSocket(`ws://${host}:${port}/logs`);

		ws.addEventListener('open', () => {
			console.log('WS Connected');
			logs += '(connected to C++)\n';
		});

		ws.addEventListener('message', (message: MessageEvent) => {
			console.log(`Received message: ${message.data}`);
			logs += `MSG: ${message.data}\n`;
			if (logsDiv !== undefined) {
				logsDiv.scrollTo(0, logsDiv.scrollHeight);
			}
		});

		ws.addEventListener('error', (error) => {
			console.log(`Received message: ${error}`);
			logs += `ERR: ${error}\n`;
		});

		ws.addEventListener('close', () => {
			console.log(`connection closed`);
			logs += '(connection closed)\n';
			createSocket();
		});
	};
	let ws = createSocket();
</script>

<div class="flex-container">
	<div class="ui-main">
		{#if !servedFromCpp}
			<p>Start the C++ server on the localhost.</p>
		{/if}
		<p>Enter some number and click the button:</p>
		<input class="my-input" type="number" bind:value={someNumber} />
		<button
			class="my-button"
			onclick={async () => {
				try {
					const data = await fetch(`http://${host}:${port}/compute1/${someNumber}`).then((x) =>
						x.json()
					);
					console.log(data);
					someResult = data.result;
				} catch (e) {
					someResult = `error, ${e}`;
				}
			}}>Ask C++</button
		>
		<p>result = {someResult}</p>
	</div>
	<div bind:this={logsDiv} class="logs">
		Logs:<br />{#each logs.split('\n') as line}
			{line}<br />
		{/each}
	</div>
</div>

<style>
	:root {
		background-color: #003;
		color: #aaf;
		font-size: 2em;
	}
	.flex-container {
		height: 100vh;
		display: flex;
		flex-direction: column;
	}
	.ui-main {
		flex: 0 0;
	}
	.logs {
		background-color: #444;
		color: #a75;
		font-family: 'Courier New', Courier, monospace;
		font-size: 0.5em;
		flex: 1 0;
		overflow-y: scroll;
	}
	.my-button {
		background-color: #9d9;
		color: #003;
		font-size: 1em;
		border-radius: 0.3em;
		border-width: 4px;
		border-color: #bfb;
	}
	.my-input {
		background-color: #afa;
		color: #003;
		font-size: 1em;
		width: 4em;
	}
</style>
