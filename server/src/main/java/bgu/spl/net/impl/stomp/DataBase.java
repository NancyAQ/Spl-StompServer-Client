package bgu.spl.net.impl.stomp;
import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.ConcurrentHashMap;

public class DataBase {
   private static DataBase db = null; // initial value of our db
   protected ConcurrentHashMap<String, List<User>> topics = new ConcurrentHashMap<>(); // every topic has a list of subs
   protected ConcurrentHashMap<Integer, User> ConnectionIdToUser = new ConcurrentHashMap<>();// every client has exactly
                                                                                             // one user!
   protected ConcurrentHashMap<User, Integer> UserToConnectionnId = new ConcurrentHashMap<>();
   protected ConcurrentHashMap<String, User> loginToUser = new ConcurrentHashMap<>();// every client has exactly one
                                                                                     // user!
   protected ConcurrentHashMap<String, User> passwordToUser = new ConcurrentHashMap<>();

   private DataBase() {// private constructor because our db is a singlton

   }

   public static DataBase getInstance() {
      if (db == null)
         return (new DataBase());
      return db;
   }

   public void addTopic(String topic) { // check if this is necessary
      if (!topics.containsKey(topic)) {
         topics.put(topic, new ArrayList<>());
      }
   }

   public void removeTopic(String topic) {// check if this is necessary too
      if (topics.containsKey(topic)) {
         topics.remove(topic);
      }
   }

   public boolean addSub(User subscriber, String Topic) {
      String subscriptionId = subscriber.GetsubscriptionId(Topic);
      subscriber.joinChannel(Topic, subscriptionId );
      String login=subscriber.login;
      User existingUser=loginToUser.get(login);
         if (!existingUser.confirmUser(subscriber))
            return false; // check this in the stomp so if theres a user it returns a error frame
      
      if (!topics.containsKey(Topic)) {
         topics.put(Topic, new ArrayList<>());
         }
      if (topics.containsKey(Topic)) {
         if (!topics.get(Topic).contains(subscriber))
            topics.get(Topic).add(subscriber);
      }

      return true;

   }

   public void removeSub(User subscriber, String Topic) {
      if (topics.containsKey(Topic)) {
         topics.get(Topic).remove(subscriber);
      }

   }

   public List<User> getSubs(String Topic) {
      return (topics.get(Topic));
   }

   public ConcurrentHashMap<String, List<User>> topics() {
      return topics;
   }

   public void dbUserDisconnect(int connectionId) {
      User user = ConnectionIdToUser.get(connectionId);
      for (String topic : user.ChannelToId.keySet()) {
         topics.remove(topic, user);// removed our user from every channel it was subscribed to
      }
      ConnectionIdToUser.remove(user.connectionId);
      UserToConnectionnId.remove(user);
      loginToUser.get(user.login).setLogged(false);
      passwordToUser.get(user.passcode).setLogged(false);
      user.disconnect();
   }

   public void AddNewUser(User user) {
      int id = user.connectionId;
      user.setLogged(true);
      ConnectionIdToUser.put(id, user);
      UserToConnectionnId.put(user, id);
      String login = user.login;
      String passcode = user.passcode;
      loginToUser.put(login, user);
      passwordToUser.put(passcode, user);
   }

   public void LoginExistingUser(User user) { 
      String login = user.login;
      String passcode = user.passcode;
      user.setLogged(true);
      ConnectionIdToUser.put(user.connectionId, user);
      UserToConnectionnId.put(user, user.connectionId);
      loginToUser.get(login).setLogged(true);
      loginToUser.get(login).connectionId = user.connectionId;
      passwordToUser.get(passcode).setLogged(true);
      passwordToUser.get(passcode).connectionId = user.connectionId;

   }

   public boolean WrongPassword(String login, String password) { // returns true if login is correct but passcode isnt
      Boolean output = false;
      if (loginToUser.containsKey(login)) {
         User usr = loginToUser.get(login);
         if (!usr.passcode.equals(password))
            output = true;
      }
      return output;

   }

   public boolean WrongName(String login, String password) {// returns true if passcode is correct but login isnt
      Boolean output = false;
      if (passwordToUser.containsKey(password)) {
         User usr = passwordToUser.get(password);
         if (!usr.login.equals(login))
            output = true;
      }
      return output;

   }

   public boolean existsAndActive(User usr) { // returns true if user is connected and false otherwise
      boolean output = false;
      String login = usr.login;
      String passcode = usr.passcode;
      if(loginToUser.containsKey(login)){
         User usr1=loginToUser.get(login);
         if(usr1.login.equals(login) & usr1.passcode.equals(passcode)){
            if(usr1.logged)
            output=true;
         }
      }
      return output;

   }

   public boolean existsButNotActive(User usr) {
      boolean output = false;
      String login = usr.login;
      String passcode = usr.passcode;
      boolean affermpass = !(WrongPassword(login, passcode));
      boolean affermlogin = !(WrongName(login, passcode));
      if (affermlogin & affermpass) {
         if (!loginToUser.get(login).logged)
            output = true;

      }
      return output;
   }

   public boolean isNewUser(User usr) {
      boolean output = false;
      String login = usr.login;
      String passcode = usr.passcode;
      if (!loginToUser.containsKey(login) && !passwordToUser.containsKey(passcode))
         output = true;
      return output;

   }

}
