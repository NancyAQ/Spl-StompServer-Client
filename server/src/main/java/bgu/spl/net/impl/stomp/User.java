package bgu.spl.net.impl.stomp;

import java.util.concurrent.ConcurrentHashMap;

public class User {
    protected String login; // username
    protected String passcode; // password
    protected ConcurrentHashMap<String, String> idToChannel;// maps channels ids to each subscribe id
    protected ConcurrentHashMap<String, String> ChannelToId;// maps channels ids to each subscribe id
    DataBase db = DataBase.getInstance();
    int connectionId;
    protected boolean logged;

    public User(String login, String passcode, int id) {
        this.login = login;
        this.passcode = passcode;
        this.idToChannel = new ConcurrentHashMap<>();
        this.ChannelToId = new ConcurrentHashMap<>();
        this.connectionId = id;
        this.logged = true;
    }

    public boolean confirmUser(User compareTo) {
        String name = compareTo.login;
        String password = compareTo.passcode;
        return (this.login.equals(name) && this.passcode.equals(password));
    }

    public String GetsubscriptionId(String channel) {
        return ChannelToId.get(channel);
    }

    public String GetChannel(String id) {

        return idToChannel.get(id);
    }

    public void joinChannel(String channel, String id) {
        idToChannel.put(id, channel);
        ChannelToId.put(channel, id);


    }

    public void leaveChannel(String channel, String id) {
        idToChannel.remove(id, channel);
        ChannelToId.remove(channel, id);
    }

    public void disconnect() {
        idToChannel.clear();
        ChannelToId.clear();
        this.logged = false; // when disconnecting a user we change its logged variable to false

    }

    public void setLogged(boolean set) {
        this.logged = set;
    }

}
