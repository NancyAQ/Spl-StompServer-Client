package bgu.spl.net.impl.stomp;
import bgu.spl.net.srv.Server;

public class StompServer {

   public static void main(String[] args) {
      if (args.length < 2)
         System.out.println("input is illegal, short of input arguments");
      else {
         int port = Integer.valueOf(args[0]);

         if ("reactor".equals(args[1])) {
            Server.reactor(
                  Runtime.getRuntime().availableProcessors(),
                  port, // port
                  StompMessagingProtocolImpl::new, // protocol factory
                  StompEncoderDecoder::new // message encoder decoder factory
            ).serve();

         } else if ("tpc".equals(args[1])) {
            Server.threadPerClient(
                  port, // port
                  StompMessagingProtocolImpl::new, // protocol factory
                  StompEncoderDecoder::new // message encoder decoder factory
            ).serve();

         }
      }
   }
}
