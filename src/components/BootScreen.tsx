import React, { useState, useEffect } from 'react';
import { motion } from 'framer-motion';

const BootScreen: React.FC = () => {
  const [progress, setProgress] = useState(0);
  const [currentStep, setCurrentStep] = useState('');

  const bootSteps = [
    'Initializing hardware...',
    'Loading kernel modules...',
    'Starting system services...',
    'Mounting filesystems...',
    'Initializing graphics...',
    'Loading desktop environment...',
    'MyOS ready!'
  ];

  useEffect(() => {
    let stepIndex = 0;
    const interval = setInterval(() => {
      if (stepIndex < bootSteps.length) {
        setCurrentStep(bootSteps[stepIndex]);
        setProgress((stepIndex + 1) / bootSteps.length * 100);
        stepIndex++;
      } else {
        clearInterval(interval);
      }
    }, 400);

    return () => clearInterval(interval);
  }, []);

  return (
    <div className="w-full h-full bg-gradient-to-br from-blue-900 via-purple-900 to-indigo-900 flex flex-col items-center justify-center text-white">
      <motion.div
        initial={{ scale: 0.8, opacity: 0 }}
        animate={{ scale: 1, opacity: 1 }}
        transition={{ duration: 0.8 }}
        className="text-center mb-16"
      >
        <div className="w-24 h-24 mx-auto mb-6 bg-gradient-to-r from-blue-400 to-purple-400 rounded-2xl flex items-center justify-center">
          <span className="text-3xl font-bold">OS</span>
        </div>
        <h1 className="text-5xl font-bold mb-2 bg-gradient-to-r from-blue-400 to-purple-400 bg-clip-text text-transparent">
          MyOS
        </h1>
        <p className="text-xl text-gray-300">Next-Generation Operating System</p>
      </motion.div>

      <div className="w-full max-w-md px-8">
        <div className="mb-4">
          <div className="w-full bg-gray-800 rounded-full h-2">
            <motion.div
              className="bg-gradient-to-r from-blue-400 to-purple-400 h-2 rounded-full"
              initial={{ width: 0 }}
              animate={{ width: `${progress}%` }}
              transition={{ duration: 0.3 }}
            />
          </div>
        </div>
        
        <motion.p
          key={currentStep}
          initial={{ opacity: 0, y: 10 }}
          animate={{ opacity: 1, y: 0 }}
          className="text-center text-gray-300"
        >
          {currentStep}
        </motion.p>
      </div>

      <div className="absolute bottom-8 text-center text-gray-400 text-sm">
        <p>© 2025 MyOS Development Team</p>
        <p>Version 1.0.0 - Build 2025.01.26</p>
      </div>
    </div>
  );
};

export default BootScreen;
