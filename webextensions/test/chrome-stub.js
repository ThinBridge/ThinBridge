function createStubMethod(name) {
  return ((...args) => {
    //console.log(`called: ${name}`, args, new Error().stack);
  });
}

global.chrome = {
    alarms: {
        create: createStubMethod('alarms.create'),
        onAlarm: {
            addListener: createStubMethod('alarms.onAlarm.addListener'),
        },
    },
    runtime: {
        sendNativeMessage: createStubMethod('runtime.sendNativeMessage'),
        lastError: null,
    },
    tabs: {
        onCreated: {
            addListener: createStubMethod('tabs.onCreated.addListener'),
        },
        onUpdated: {
            addListener: createStubMethod('tabs.onUpdated.addListener'),
        },
        query: createStubMethod('tabs.query'),
        get: createStubMethod('tabs.get'),
        remove: createStubMethod('tabs.remove'),
    },
    webRequest: {
        onBeforeRequest: {
            addListener: createStubMethod('webRequest.onBeforeRequest.addListener'),
        },
    },
    webNavigation: {
        onCommitted: {
            addListener: createStubMethod('webNavigation.onCommitted.addListener'),
        },
    },
    storage: {
      session: {
        get: createStubMethod('storage.session.get'),
        set: createStubMethod('storage.session.set'),
      },
    },
    scripting: {
        executeScript: createStubMethod('tabs.remove'),
    },
};
