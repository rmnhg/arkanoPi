package es.ramonhg.arkanopi.ui.network;


public class ConnectTask extends Thread {
    TCPClient mTCPClient;

    public ConnectTask(TCPClient mTCPClient) {
        this.mTCPClient = mTCPClient;
    }

    @Override
    public void run() {
        mTCPClient.run();
    }
}