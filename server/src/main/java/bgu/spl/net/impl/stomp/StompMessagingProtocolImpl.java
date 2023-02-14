package bgu.spl.net.impl.stomp;

import java.util.List;
import java.util.Map;
import java.util.concurrent.atomic.AtomicInteger;
import bgu.spl.net.api.StompMessagingProtocol;
import bgu.spl.net.srv.Connections;

public class StompMessagingProtocolImpl implements StompMessagingProtocol<String> {
    private int connectionId;
    private Connections<String> connections;
    private boolean shouldTerminate = false;
    private AtomicInteger messageid;
    private final String acceptVersion = "1.2";
    private final String hostName = "stomp.cs.ac.il";

    @Override
    public void start(int connectionId, Connections<String> connections) {
        this.connectionId = connectionId;
        this.connections = connections;
        messageid = new AtomicInteger(0);
    }

    @Override
    public void process(String message) {
        FrameParser parse = new FrameParser((String) message);
        String command = parse.GetCommand();
        Map<String, String> headers = parse.getHeaders();
        String framebody = parse.getBody();
        switch (command) {
            case "CONNECT":
                handleConnect(headers);
                break;
            case "SEND":
                handleSend(headers, framebody);
                break;
            case "SUBSCRIBE":
                handleSubscribe(headers);
                break;
            case "UNSUBSCRIBE":
                handleUnSubscribe(headers);
                break;
            case "DISCONNECT":
                handleDisconnect(headers);
                break;

        }
    }

    private void handleConnect(Map<String, String> headers) { 
        String acceptv = headers.get("accept-version"); 
        String login = headers.get("login");
        String passcode = headers.get("passcode");
            User toConnect = new User(login, passcode, this.connectionId);
            if (connections.existsAndActive(toConnect)) {
                String error = "ERROR" + "\n" + "message:User already logged in" + "\n" + "\n" + '\u0000';
                connections.send(connectionId, error);

            } else if (connections.WrongName(login, passcode)) {
                String error = "ERROR" + "\n" + "message:Wrong login" + "\n" + "\n"+ '\u0000';
                connections.send(connectionId, error);

            } else if (connections.WrongPassword(login, passcode)) {
                String error = "ERROR" + "\n" + "message:Wrong password" + "\n" + "\n" + '\u0000';
                connections.send(connectionId, error);

            } else if (connections.isNewUser(toConnect)) {
                connections.AddNewUser(toConnect);
                String connected = "CONNECTED" + "\n" + "version:" + acceptVersion + "\n" + "\n" + '\u0000';
                connections.send(connectionId, connected);

            } else if (connections.existsButNotActive(toConnect)) {
                connections.LoginExistingUser(toConnect);
                String connected = "CONNECTED" + "\n" + "version:" + acceptVersion + "\n" + "\n" + '\u0000';
                connections.send(connectionId, connected);
            }
    }

    private void handleSend(Map<String, String> headers, String body) {                                                          
        // add the id of every user that recieves your message in that specific topic
        String topic = headers.get("destination"); 
        String msgbody = body;
        List<User> usertToSendTo = connections.getSubs(topic);
        if(usertToSendTo == null){
            String message = "There are no Subscribers for this destenation";
            System.out.println(message);
        }
        else{
        for (User usr : usertToSendTo) {
            String subId = usr.GetsubscriptionId(topic);
            String msgid = Integer.toString(messageid.getAndIncrement());
            String message = "MESSAGE" + "\n" + "subscription:" + subId + "\n" + "message-id:" + msgid + "\n"
            + "destination:" + topic + "\n" + "\n" + msgbody + "\n" + "\n" + '\u0000';
            connections.send(usr.connectionId, message);

        }
    }

    }

    private void handleUnSubscribe(Map<String, String> headers) {
        String subId = headers.get("id");
        String receipt = headers.get("receipt");
        connections.removeSub(connectionId, subId);
        String receiptFrame = "RECEIPT" + "\n" + "receipt-id:" + receipt + "\n" + "\n" + '\u0000';
        connections.send(connectionId, receiptFrame);
    }

    private void handleSubscribe(Map<String, String> headers) {
        String topic = headers.get("destination");
        String subId = headers.get("id");
        String receipt = headers.get("receipt");
        User usr = connections.getUser(connectionId);
        if (connections.existsAndActive(usr)) {
            usr.ChannelToId.put(topic, subId);
            if (!usr.ChannelToId.contains(topic)) {
                usr.ChannelToId.put(topic, subId);
                connections.addSub(usr, topic);
                String receiptframe = "RECEIPT" + "\n" + "receipt-id:" + receipt + "\n" + "\n" + '\u0000';
                connections.send(connectionId, receiptframe);
            }
        } else { // send error frame 
            String error = "ERROR" + "\n" + "message:cant subscribe if you're not a user" + "\n" + "\n" + '\u0000';
            connections.send(connectionId, error);
        }
    }

    private void handleDisconnect(Map<String, String> headers) {
        String receipt = headers.get("receipt");
        String receiptframe = "RECEIPT" + "\n" + "receipt-id:" + receipt + "\n" + "\n" + '\u0000';
        connections.send(connectionId, receiptframe);
        connections.disconnect(connectionId);
    }

    @Override
    public boolean shouldTerminate() {
        return shouldTerminate;
    }

}
