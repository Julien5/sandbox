import 'package:flutter/material.dart';
import 'package:nano/src/rust/api/midi.dart';
import 'midi_signal_screen.dart';

class DeviceListScreen extends StatefulWidget {
  const DeviceListScreen({super.key});

  @override
  State<DeviceListScreen> createState() => _DeviceListScreenState();
}

class _DeviceListScreenState extends State<DeviceListScreen> {
  List<String> _ports = [];
  bool _loading = true;
  String? _error;

  @override
  void initState() {
    super.initState();
    _loadPorts();
  }

  Future<void> _loadPorts() async {
    setState(() {
      _loading = true;
      _error = null;
    });
    try {
      final ports = listMidiPorts();
      setState(() {
        _ports = ports;
        _loading = false;
      });
    } catch (e) {
      setState(() {
        _error = e.toString();
        _loading = false;
      });
    }
  }

  Future<void> _connect(int index) async {
    try {
      final portName = await connectMidi(portIndex: index);
      if (!mounted) return;
      final stream = startMidiEventStream();
      Navigator.push(
        context,
        MaterialPageRoute(
          builder: (_) => MidiSignalScreen(
            portName: portName,
            eventStream: stream,
          ),
        ),
      );
    } catch (e) {
      if (!mounted) return;
      ScaffoldMessenger.of(context).showSnackBar(
        SnackBar(content: Text('Connection failed: $e')),
      );
    }
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(title: const Text('MIDI Devices')),
      body: _buildBody(),
    );
  }

  Widget _buildBody() {
    if (_loading) {
      return const Center(child: CircularProgressIndicator());
    }

    if (_error != null) {
      return Center(
        child: Column(
          mainAxisAlignment: MainAxisAlignment.center,
          children: [
            Text('Error: $_error'),
            const SizedBox(height: 16),
            ElevatedButton(
              onPressed: _loadPorts,
              child: const Text('Retry'),
            ),
          ],
        ),
      );
    }

    if (_ports.isEmpty) {
      return const Center(child: Text('No MIDI devices found'));
    }

    return ListView.builder(
      itemCount: _ports.length,
      itemBuilder: (context, index) {
        return ListTile(
          title: Text(_ports[index]),
          trailing: const Icon(Icons.chevron_right),
          onTap: () => _connect(index),
        );
      },
    );
  }
}
