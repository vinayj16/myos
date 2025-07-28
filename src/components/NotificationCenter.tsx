import React from 'react';
import { motion } from 'framer-motion';
import { useNotifications } from '../contexts/NotificationContext';
import { Bell, X, Trash2 } from 'lucide-react';

interface NotificationCenterProps {
  onClose: () => void;
}

const NotificationCenter: React.FC<NotificationCenterProps> = ({ onClose }) => {
  const { notifications, removeNotification, clearNotifications } = useNotifications();

  return (
    <motion.div
      initial={{ opacity: 0, x: '100%' }}
      animate={{ opacity: 1, x: 0 }}
      exit={{ opacity: 0, x: '100%' }}
      transition={{ duration: 0.3, ease: 'easeInOut' }}
      className="fixed top-0 right-0 h-full w-96 bg-gray-800/80 backdrop-blur-lg shadow-2xl z-40 flex flex-col border-l border-gray-700"
    >
      <div className="flex items-center justify-between p-4 border-b border-gray-700">
        <div className="flex items-center">
          <Bell className="h-5 w-5 text-white mr-2" />
          <h2 className="text-lg font-semibold text-white">Notifications</h2>
        </div>
        <button onClick={onClose} className="p-1 rounded-full hover:bg-gray-700">
          <X className="h-5 w-5 text-white" />
        </button>
      </div>

      <div className="flex-1 overflow-y-auto p-2">
        {notifications.length === 0 ? (
          <div className="text-center text-gray-400 mt-10">
            <Bell className="h-12 w-12 mx-auto mb-4 opacity-50" />
            <p>No new notifications</p>
          </div>
        ) : (
          <div className="space-y-2">
            {notifications.map(notif => (
              <motion.div
                key={notif.id}
                layout
                initial={{ opacity: 0, y: 20 }}
                animate={{ opacity: 1, y: 0 }}
                exit={{ opacity: 0, x: -50 }}
                className="bg-gray-700/50 p-3 rounded-lg"
              >
                <div className="flex justify-between items-start">
                  <div>
                    <h3 className="font-semibold text-white">{notif.title}</h3>
                    <p className="text-sm text-gray-300">{notif.message}</p>
                    <p className="text-xs text-gray-500 mt-1">{notif.timestamp.toLocaleTimeString()}</p>
                  </div>
                  <button onClick={() => removeNotification(notif.id)} className="p-1 text-gray-400 hover:text-white">
                    <X size={16} />
                  </button>
                </div>
              </motion.div>
            ))}
          </div>
        )}
      </div>

      {notifications.length > 0 && (
        <div className="p-4 border-t border-gray-700">
          <button onClick={clearNotifications} className="w-full flex items-center justify-center p-2 rounded-lg bg-red-500/20 text-red-400 hover:bg-red-500/40">
            <Trash2 size={16} className="mr-2" />
            Clear All Notifications
          </button>
        </div>
      )}
    </motion.div>
  );
};

export default NotificationCenter;
