package bgu.spl.net.impl.stomp;
import java.util.HashMap;
import java.util.Map;
public class FrameParser {
  private String message;
  public FrameParser(String message) {
   this.message=message;
 }
   public String GetCommand() {
    String message=this.message;
    String[] parts = message.split("\n");
    return parts[0];
}
public Map<String, String> getHeaders() {
    String frame=this.message;
    Map<String, String> headers = new HashMap<>();
    String[] parts = frame.split("\n");
    for (int i = 1; i < parts.length; i++) {
        if (parts[i].contains(":")) {
            String[] headerParts = parts[i].split(":");
            headers.put(headerParts[0], headerParts[1]);
        } else {
            break;
        }
    }
    return headers;
}

public String getBody() {
    String message=this.message;
    String[] parts = message.split("\n\n");
    if (parts.length > 1) {
        return parts[1];
    } else {
        return "";
    }
}




}
