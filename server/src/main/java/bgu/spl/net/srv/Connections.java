package bgu.spl.net.srv;
import java.util.List;

import bgu.spl.net.impl.stomp.User;

public interface Connections<T> {

    boolean send(int connectionId, T msg);
    void send(String channel, T msg);
    void disconnect(int connectionId);
    boolean addsub(String channel, User subscriber);
    void removeSub(int connectionId,String subscriptionId);
   // boolean addnewsub(User user);
    //boolean contain(User user);
    User getUser(int connectionId);
    void addHandler(ConnectionHandler<T> handler, int connectionId);
    void removeHandler(ConnectionHandler<T> handler, int connectionId);
    int GenerateAndIncrement();
    void AddNewUser(User user);
    boolean existsAndActive(User usr);
    boolean existsButNotActive(User usr);
    boolean isNewUser(User usr);
    boolean WrongName(String login, String password);
    boolean WrongPassword(String login, String password);
    void LoginExistingUser(User user);
    void dbUserDisconnect(int connectionId);
    List<User> getSubs(String Topic);
    public boolean addSub(User subscriber, String Topic);
   
}
