import React, { useState, useEffect } from 'react';
import { motion } from 'framer-motion';
import { faker } from '@faker-js/faker';
import { 
  Cpu, 
  HardDrive, 
  MemoryStick, 
  Thermometer, 
  Activity,
  Zap,
  Wifi,
  Monitor
} from 'lucide-react';

interface SystemData {
  cpu: number;
  memory: number;
  disk: number;
  temperature: number;
  networkUp: number;
  networkDown: number;
  uptime: string;
}

const SystemMonitor: React.FC = () => {
  const [systemData, setSystemData] = useState<SystemData>({
    cpu: 0,
    memory: 0,
    disk: 0,
    temperature: 0,
    networkUp: 0,
    networkDown: 0,
    uptime: '0:00:00'
  });

  const [history, setHistory] = useState<number[][]>([[], [], []]);

  useEffect(() => {
    const interval = setInterval(() => {
      const newData: SystemData = {
        cpu: Math.max(5, Math.min(95, systemData.cpu + (Math.random() - 0.5) * 20)),
        memory: Math.max(20, Math.min(90, systemData.memory + (Math.random() - 0.5) * 10)),
        disk: Math.max(30, Math.min(85, systemData.disk + (Math.random() - 0.5) * 5)),
        temperature: Math.max(35, Math.min(75, systemData.temperature + (Math.random() - 0.5) * 5)),
        networkUp: parseFloat((Math.random() * 10).toFixed(1)),
        networkDown: parseFloat((Math.random() * 50).toFixed(1)),
        uptime: formatUptime(Date.now())
      };

      setSystemData(newData);

      setHistory(prev => [
        [...prev[0], newData.cpu].slice(-20),
        [...prev[1], newData.memory].slice(-20),
        [...prev[2], newData.disk].slice(-20)
      ]);
    }, 1000);

    return () => clearInterval(interval);
  }, [systemData]);

  const formatUptime = (timestamp: number) => {
    const seconds = Math.floor(timestamp / 1000) % 60;
    const minutes = Math.floor(timestamp / 60000) % 60;
    const hours = Math.floor(timestamp / 3600000);
    return `${hours}:${minutes.toString().padStart(2, '0')}:${seconds.toString().padStart(2, '0')}`;
  };

  const renderMiniChart = (data: number[], color: string) => {
    const max = Math.max(...data, 1);
    const points = data.map((value, index) => 
      `${index * (180 / (data.length - 1))},${60 - (value / max) * 50}`
    ).join(' ');

    return (
      <svg className="w-full h-16" viewBox="0 0 180 60">
        <polyline
          points={points}
          fill="none"
          stroke={color}
          strokeWidth="2"
          className="opacity-80"
        />
        <polyline
          points={`0,60 ${points} ${(data.length - 1) * (180 / (data.length - 1))},60`}
          fill={color}
          className="opacity-20"
        />
      </svg>
    );
  };

  const MetricCard: React.FC<{
    title: string;
    value: number;
    unit: string;
    icon: React.ReactNode;
    color: string;
    history?: number[];
  }> = ({ title, value, unit, icon, color, history }) => (
    <motion.div
      initial={{ opacity: 0, y: 20 }}
      animate={{ opacity: 1, y: 0 }}
      className="bg-white dark:bg-gray-800 rounded-lg p-4 border border-gray-200 dark:border-gray-700"
    >
      <div className="flex items-center justify-between mb-2">
        <div className="flex items-center">
          <div className={`p-2 rounded-lg ${color} mr-3`}>
            {icon}
          </div>
          <div>
            <h3 className="font-semibold text-gray-900 dark:text-gray-100">{title}</h3>
            <p className="text-2xl font-bold text-gray-900 dark:text-gray-100">
              {value.toFixed(unit === '°C' ? 0 : 1)}{unit}
            </p>
          </div>
        </div>
      </div>
      
      {history && history.length > 1 && (
        <div className="mt-3">
          {renderMiniChart(history, color.includes('blue') ? '#3B82F6' : 
                                  color.includes('green') ? '#10B981' : 
                                  color.includes('yellow') ? '#F59E0B' : '#EF4444')}
        </div>
      )}

      <div className="mt-2">
        <div className={`w-full bg-gray-200 dark:bg-gray-700 rounded-full h-2`}>
          <motion.div
            initial={{ width: 0 }}
            animate={{ width: `${value}%` }}
            transition={{ duration: 0.5 }}
            className={`h-2 rounded-full ${
              value > 80 ? 'bg-red-500' : 
              value > 60 ? 'bg-yellow-500' : 
              'bg-green-500'
            }`}
          />
        </div>
      </div>
    </motion.div>
  );

  const processes = Array.from({ length: 8 }, () => ({
    name: faker.system.fileName(),
    pid: faker.number.int({ min: 100, max: 9999 }),
    cpu: parseFloat((Math.random() * 25).toFixed(1)),
    memory: parseFloat((Math.random() * 15).toFixed(1)),
    status: faker.helpers.arrayElement(['Running', 'Sleeping', 'Waiting'])
  }));

  return (
    <div className="h-full bg-gray-50 dark:bg-gray-900 overflow-auto">
      <div className="p-6">
        <div className="flex items-center mb-6">
          <Activity className="h-6 w-6 text-blue-500 mr-2" />
          <h1 className="text-2xl font-bold text-gray-900 dark:text-gray-100">System Monitor</h1>
        </div>

        {/* System Overview */}
        <div className="grid grid-cols-1 md:grid-cols-2 lg:grid-cols-4 gap-4 mb-6">
          <MetricCard
            title="CPU Usage"
            value={systemData.cpu}
            unit="%"
            icon={<Cpu className="h-5 w-5 text-white" />}
            color="bg-blue-500"
            history={history[0]}
          />
          
          <MetricCard
            title="Memory"
            value={systemData.memory}
            unit="%"
            icon={<MemoryStick className="h-5 w-5 text-white" />}
            color="bg-green-500"
            history={history[1]}
          />
          
          <MetricCard
            title="Disk Usage"
            value={systemData.disk}
            unit="%"
            icon={<HardDrive className="h-5 w-5 text-white" />}
            color="bg-yellow-500"
            history={history[2]}
          />
          
          <MetricCard
            title="Temperature"
            value={systemData.temperature}
            unit="°C"
            icon={<Thermometer className="h-5 w-5 text-white" />}
            color="bg-red-500"
          />
        </div>

        {/* Network & System Info */}
        <div className="grid grid-cols-1 md:grid-cols-2 gap-6 mb-6">
          <motion.div
            initial={{ opacity: 0, x: -20 }}
            animate={{ opacity: 1, x: 0 }}
            className="bg-white dark:bg-gray-800 rounded-lg p-4 border border-gray-200 dark:border-gray-700"
          >
            <div className="flex items-center mb-4">
              <Wifi className="h-5 w-5 text-blue-500 mr-2" />
              <h3 className="font-semibold text-gray-900 dark:text-gray-100">Network Activity</h3>
            </div>
            <div className="space-y-3">
              <div className="flex justify-between">
                <span className="text-gray-600 dark:text-gray-400">Upload:</span>
                <span className="font-medium text-gray-900 dark:text-gray-100">
                  {systemData.networkUp} MB/s
                </span>
              </div>
              <div className="flex justify-between">
                <span className="text-gray-600 dark:text-gray-400">Download:</span>
                <span className="font-medium text-gray-900 dark:text-gray-100">
                  {systemData.networkDown} MB/s
                </span>
              </div>
            </div>
          </motion.div>

          <motion.div
            initial={{ opacity: 0, x: 20 }}
            animate={{ opacity: 1, x: 0 }}
            className="bg-white dark:bg-gray-800 rounded-lg p-4 border border-gray-200 dark:border-gray-700"
          >
            <div className="flex items-center mb-4">
              <Monitor className="h-5 w-5 text-green-500 mr-2" />
              <h3 className="font-semibold text-gray-900 dark:text-gray-100">System Info</h3>
            </div>
            <div className="space-y-3">
              <div className="flex justify-between">
                <span className="text-gray-600 dark:text-gray-400">Uptime:</span>
                <span className="font-medium text-gray-900 dark:text-gray-100">
                  {systemData.uptime}
                </span>
              </div>
              <div className="flex justify-between">
                <span className="text-gray-600 dark:text-gray-400">OS:</span>
                <span className="font-medium text-gray-900 dark:text-gray-100">
                  MyOS 1.0.0
                </span>
              </div>
              <div className="flex justify-between">
                <span className="text-gray-600 dark:text-gray-400">Kernel:</span>
                <span className="font-medium text-gray-900 dark:text-gray-100">
                  5.15.0-myos
                </span>
              </div>
            </div>
          </motion.div>
        </div>

        {/* Process List */}
        <motion.div
          initial={{ opacity: 0, y: 20 }}
          animate={{ opacity: 1, y: 0 }}
          className="bg-white dark:bg-gray-800 rounded-lg border border-gray-200 dark:border-gray-700"
        >
          <div className="p-4 border-b border-gray-200 dark:border-gray-700">
            <h3 className="font-semibold text-gray-900 dark:text-gray-100">Running Processes</h3>
          </div>
          <div className="overflow-x-auto">
            <table className="w-full">
              <thead className="bg-gray-50 dark:bg-gray-700">
                <tr>
                  <th className="px-4 py-2 text-left text-xs font-medium text-gray-500 dark:text-gray-400 uppercase tracking-wider">
                    Process
                  </th>
                  <th className="px-4 py-2 text-left text-xs font-medium text-gray-500 dark:text-gray-400 uppercase tracking-wider">
                    PID
                  </th>
                  <th className="px-4 py-2 text-left text-xs font-medium text-gray-500 dark:text-gray-400 uppercase tracking-wider">
                    CPU %
                  </th>
                  <th className="px-4 py-2 text-left text-xs font-medium text-gray-500 dark:text-gray-400 uppercase tracking-wider">
                    Memory %
                  </th>
                  <th className="px-4 py-2 text-left text-xs font-medium text-gray-500 dark:text-gray-400 uppercase tracking-wider">
                    Status
                  </th>
                </tr>
              </thead>
              <tbody className="divide-y divide-gray-200 dark:divide-gray-700">
                {processes.map((process, index) => (
                  <motion.tr
                    key={index}
                    initial={{ opacity: 0 }}
                    animate={{ opacity: 1 }}
                    transition={{ delay: index * 0.05 }}
                    className="hover:bg-gray-50 dark:hover:bg-gray-700"
                  >
                    <td className="px-4 py-2 text-sm font-medium text-gray-900 dark:text-gray-100">
                      {process.name}
                    </td>
                    <td className="px-4 py-2 text-sm text-gray-600 dark:text-gray-400">
                      {process.pid}
                    </td>
                    <td className="px-4 py-2 text-sm text-gray-600 dark:text-gray-400">
                      {process.cpu}%
                    </td>
                    <td className="px-4 py-2 text-sm text-gray-600 dark:text-gray-400">
                      {process.memory}%
                    </td>
                    <td className="px-4 py-2 text-sm">
                      <span className={`px-2 py-1 text-xs rounded-full ${
                        process.status === 'Running' ? 'bg-green-100 text-green-800 dark:bg-green-900 dark:text-green-200' :
                        process.status === 'Sleeping' ? 'bg-yellow-100 text-yellow-800 dark:bg-yellow-900 dark:text-yellow-200' :
                        'bg-gray-100 text-gray-800 dark:bg-gray-900 dark:text-gray-200'
                      }`}>
                        {process.status}
                      </span>
                    </td>
                  </motion.tr>
                ))}
              </tbody>
            </table>
          </div>
        </motion.div>
      </div>
    </div>
  );
};

export default SystemMonitor;
