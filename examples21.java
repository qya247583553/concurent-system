import org.cloudbus.cloudsim.Cloudlet;
import org.cloudbus.cloudsim.CloudletScheduler;
import org.cloudbus.cloudsim.CloudletSchedulerSpaceShared;
import org.cloudbus.cloudsim.Datacenter;
import org.cloudbus.cloudsim.DatacenterBroker;
import org.cloudbus.cloudsim.Host;
import org.cloudbus.cloudsim.Log;
import org.cloudbus.cloudsim.Pe;
import org.cloudbus.cloudsim.Storage;
import org.cloudbus.cloudsim.Vm;
import org.cloudbus.cloudsim.VmAllocationPolicySimple;
import org.cloudbus.cloudsim.VmScheduler;
import org.cloudbus.cloudsim.VmSchedulerSpaceShared;
import org.cloudbus.cloudsim.core.CloudSim;

import java.util.ArrayList;
import java.util.LinkedList;
import java.util.List;

public class examples21 {
    private static final int NUM_DATA_CENTERS = 5;
    private static final int NUM_HOSTS_PER_DATA_CENTER = 2;
    private static final int NUM_PES_PER_HOST = 2;
    private static final int HOST_MIPS = 1000;
    private static final int HOST_RAM = 2048;
    private static final long HOST_STORAGE = 1000000;
    private static final int VM_MIPS = 500;
    private static final int VM_RAM = 1024;
    private static final long VM_STORAGE = 100000;
    private static final int CLOUDLET_LENGTH = 1000;
    private static final int NUM_CLOUDLETS = 20;
    private static final int SIMULATION_TIME = 1000;

    public static void main(String[] args) {
        try {
            Log.printConcatLine("Starting CloudSimExample...");

            CloudSim.init(1, Calendar.getInstance(), false);

            DatacenterBroker broker = createBroker();
            int brokerId = broker.getId();

            List<Datacenter> datacenterList = createDatacenters();
            broker.setDatacenterList(datacenterList);

            List<Vm> vmList = createVms(brokerId);
            broker.submitVmList(vmList);

            List<Cloudlet> cloudletList = createCloudlets(brokerId);
            broker.submitCloudletList(cloudletList);

            CloudSim.startSimulation();

            CloudSim.stopSimulation();

            List<Cloudlet> finishedCloudlets = broker.getCloudletFinishedList();
            printCloudletList(finishedCloudlets);
            Log.printConcatLine("CloudSimExample finished!");
        } catch (Exception e) {
            e.printStackTrace();
            Log.printConcatLine("Unwanted errors happen");
        }
    }

    private static DatacenterBroker createBroker() throws Exception {
        return new DatacenterBroker("Broker");
    }

    private static List<Datacenter> createDatacenters() {
        List<Datacenter> datacenterList = new ArrayList<>(NUM_DATA_CENTERS);
        for (int i = 0; i < NUM_DATA_CENTERS; i++) {
            List<Host> hostList = createHosts();
            VmAllocationPolicySimple vmAllocationPolicy = new VmAllocationPolicySimple(hostList);
            List<Pe> peList = new ArrayList<>(NUM_PES_PER_HOST);
            for (int j = 0; j < NUM_PES_PER_HOST; j++) {
                peList.add(new Pe(j, new PeProvisionerSimple(HOST_MIPS)));
            }
            VmScheduler vmScheduler = new Vm
            SchedulerSpaceShared();
            List<Vm> vmList = new ArrayList<>(NUM_HOSTS_PER_DATA_CENTER);
            for (int j = 0; j < NUM_HOSTS_PER_DATA_CENTER; j++) {
                vmList.add(new Vm(j, brokerId, VM_MIPS, NUM_PES_PER_HOST, VM_RAM, VM_STORAGE, "Xen", vmScheduler));
            }
            Datacenter datacenter = new Datacenter("Datacenter_" + i, hostList, vmAllocationPolicy);
            datacenter.setVmList(vmList);
            datacenterList.add(datacenter);
        }
        return datacenterList;
    }
    private static List<Host> createHosts() {
        List<Host> hostList = new ArrayList<>(NUM_HOSTS_PER_DATA_CENTER);
        for (int i = 0; i < NUM_HOSTS_PER_DATA_CENTER; i++) {
            hostList.add(new Host(i, new RamProvisionerSimple(HOST_RAM), new BwProvisionerSimple(1000000), HOST_STORAGE, createPes()));
        }
        return hostList;
    }

    private static List<Pe> createPes() {
        List<Pe> peList = new ArrayList<>(NUM_PES_PER_HOST);
        for (int i = 0; i < NUM_PES_PER_HOST; i++) {
            peList.add(new Pe(i, new PeProvisionerSimple(HOST_MIPS)));
        }
        return peList;
    }

    private static List<Vm> createVms(int brokerId) {
        List<Vm> vmList = new ArrayList<>(NUM_HOSTS_PER_DATA_CENTER * NUM_DATA_CENTERS);
        for (int i = 0; i < NUM_HOSTS_PER_DATA_CENTER * NUM_DATA_CENTERS; i++) {
            VmScheduler vmScheduler = new VmSchedulerSpaceShared();
            vmList.add(new Vm(i, brokerId, VM_MIPS, NUM_PES_PER_HOST, VM_RAM, VM_STORAGE, "Xen", vmScheduler));
        }
        return vmList;
    }

    private static List<Cloudlet> createCloudlets(int brokerId) {
        List<Cloudlet> cloudletList = new ArrayList<>(NUM_CLOUDLETS);
        for (int i = 0; i < NUM_CLOUDLETS; i++) {
            long cloudletLength = CLOUDLET_LENGTH * (i + 1);
            int pesNumber = 1;
            CloudletScheduler cloudletScheduler = new CloudletSchedulerSpaceShared();
            cloudletList.add(new Cloudlet(i, cloudletLength, pesNumber, VM_RAM, VM_STORAGE, cloudletScheduler, brokerId));
        }
        return cloudletList;
    }

    private static void printCloudletList(List<Cloudlet> list) {
        int size = list.size();
        Cloudlet cloudlet;
        String indent = "    ";
        Log.printConcatLine();
        Log.printConcatLine("========== OUTPUT ==========");
        Log.printConcatLine("Cloudlet ID" + indent + "STATUS" + indent + "Data center ID" + indent + "VM ID" + indent + "Time" + indent + "Start Time" + indent + "Finish Time");
        for (int i = 0; i < size; i++) {
            cloudlet = list.get(i);
            Log.printConcatLine(indent + cloudlet.getCloudletId() + indent + indent + cloudlet.getStatusString() + indent + indent + cloudlet.getResourceId() + indent + indent + indent + cloudlet.getVmId() + indent + indent + indent + cloudlet.getActualCPUTime() + indent + indent + indent + cloudlet.getExecStartTime() + indent +
                    indent + indent + indent + cloudlet.getFinishTime());
        }
    }
}

class SJFScheduler extends VmScheduler {
    private List<Cloudlet> waitingList;



    public SJFScheduler(List<? extends Vm> vmList) {
        super(vmList);
        waitingList = new LinkedList<>();
    }

    @Override
    public void updateVmProcessing(double currentTime, List<Double> mipsShare) {
        if (waitingList.isEmpty()) {
            return;
        }

        Cloudlet nextCloudlet = waitingList.stream()
                .min((c1, c2) -> Double.compare(c1.getLength(), c2.getLength()))
                .orElse(null);

        if (nextCloudlet == null) {
            return;
        }

        Vm vm = findVmForCloudlet(nextCloudlet);
        if (vm == null) {
            return;
        }

        double estimatedFinishTime = currentTime + nextCloudlet.getLength() / (vm.getMips() * vm.getNumberOfPes());
        send(getVmsToDatacentersMap().get(vm.getId()), estimatedFinishTime, CloudSimTags.CLOUDLET_SUBMIT, nextCloudlet);
        waitingList.remove(nextCloudlet);
    }

    @Override
    public boolean allocateCloudletToVm(Cloudlet cloudlet, Vm vm) {
        if (vm.isSuitableForCloudlet(cloudlet)) {
            waitingList.add(cloudlet);
            return true;
        }
        return false;
    }

    private Vm findVmForCloudlet(Cloudlet cloudlet) {
        for (Vm vm : getVmList()) {
            if (vm.isSuitableForCloudlet(cloudlet)) {
                return vm;
            }
        }
        return null;
    }
