import 'package:flutter/widgets.dart';
import 'package:provider/provider.dart';
import 'package:ui/src/backendmodel.dart';

class EventProvider extends StatelessWidget {
  final EventModel? model;
  const EventProvider({super.key, this.model});

  @override
  Widget build(BuildContext ctx) {
    return ChangeNotifierProvider.value(
      value: model,
      builder: (context, child) {
        return EventConsumer();
      },
    );
  }
}

class EventConsumer extends StatefulWidget {
  const EventConsumer({super.key});

  @override
  State<EventConsumer> createState() => _EventConsumerState();
}

class _EventConsumerState extends State<EventConsumer> {
  late Stream<String> ticks;

  @override
  void initState() {
    super.initState();
  }

  @override
  Widget build(BuildContext ctx) {
    return Consumer<EventModel>(
      builder: (context, model, child) {
        String? event = model.lastEvent();
        return Text("event:$event");
      },
    );
  }
}
