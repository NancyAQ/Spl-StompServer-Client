package bgu.spl.net.impl.stomp;
import java.util.List;
import java.util.Map;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.atomic.AtomicInteger;
import bgu.spl.net.srv.ConnectionHandler;
import bgu.spl.net.srv.Connections;

public class ConnectionsImpl<T> implements Connections<T> {
    private Map<Integer, ConnectionHandler<T>> connectionHandlers;
    private DataBase db;
    private AtomicInteger connectionId; // creating new atomic integer so we dont use synchronization on regular
                                        // integers(starts from 0)

    public ConnectionsImpl() {
        connectionHandlers = new ConcurrentHashMap<>();
        db = DataBase.getInstance();
        connectionId = new AtomicInteger(0);
    }

    @Override
    public boolean addsub(String channel, User subscriber) {
        return db.addSub(subscriber, channel);
    }

    @Override
    public void removeSub(int connectionId, String subscriptionId) {
        User user = db.ConnectionIdToUser.get(connectionId);
        String ChannelToLeave = user.GetChannel(subscriptionId);
        db.removeSub(user, ChannelToLeave);
        user.leaveChannel(ChannelToLeave, subscriptionId);

    }

    @Override
    public boolean send(int connectionId, T msg) {
        boolean output = true;
        ConnectionHandler<T> connection = connectionHandlers.get(connectionId);
        if (connection == null) {
            output = false;
        } else {
            connection.send(msg);
        }
        return output;
    }

    @Override
    public void send(String channel, T msg) {
        List<User> subscribers = db.getSubs(channel);
        for (User subscriber : subscribers) {
            ConnectionHandler<T> connection = connectionHandlers.get(subscriber.connectionId);
            if (connection != null) {
                connection.send(msg);
            }
        }
    }

    @Override
    public void disconnect(int connectionId) {
        if (connectionHandlers.containsKey(connectionId)) {
            db.dbUserDisconnect(connectionId);
            connectionHandlers.remove(connectionId);
        }
    }

    @Override
    public void addHandler(ConnectionHandler<T> handler, int connectionId) {
        connectionHandlers.put(connectionId, handler);
    }

    @Override
    public void removeHandler(ConnectionHandler<T> handler, int connectionId) {
        connectionHandlers.remove(connectionId, handler);
    }

    @Override
    public User getUser(int connectionId) {
        return db.ConnectionIdToUser.get(connectionId);
    }

    @Override
    public int GenerateAndIncrement() {
        return this.connectionId.getAndIncrement();
    }
    ///handle singleton issue
    @Override
    public void AddNewUser(User user){
        db.AddNewUser(user);
    }
    @Override
    public boolean existsAndActive(User usr) { 
        return db.existsAndActive(usr);
    }
    @Override
    public boolean existsButNotActive(User usr) {
        return db.existsButNotActive(usr);
    }
    @Override
    public boolean isNewUser(User usr) {
        return db.isNewUser(usr);
    }
    @Override
    public boolean WrongName(String login, String password) {
        return db.WrongName(login, password);
    }
    @Override
    public boolean WrongPassword(String login, String password) {
        return db.WrongPassword(login, password);
    }
    @Override
    public void LoginExistingUser(User user) { 
        db.LoginExistingUser(user);
    }
    @Override
    public void dbUserDisconnect(int connectionId) {
        db.dbUserDisconnect(connectionId);
    }
    @Override
    public List<User> getSubs(String Topic){
       return db.getSubs(Topic);
    }
    @Override
    public boolean addSub(User subscriber, String Topic){
      return db.addSub(subscriber, Topic);
    }


}
