import React from 'react';
import { motion } from 'framer-motion';

const Wallpaper: React.FC = () => {
  return (
    <div className="absolute inset-0 overflow-hidden bg-gray-900">
      <motion.div
        className="absolute inset-[-200%] anm-aurora"
        style={{
          backgroundImage:
            'radial-gradient(ellipse at 50% 50%, rgba(29, 78, 216, 0.5) 0%, rgba(255, 255, 255, 0) 100%), radial-gradient(ellipse at 50% 50%, rgba(124, 58, 237, 0.4) 0%, rgba(255, 255, 255, 0) 100%)',
        }}
        animate={{
          backgroundPosition: ['50% 50%', '100% 50%', '50% 50%'],
          backgroundSize: ['150% 150%', '120% 120%', '150% 150%'],
        }}
        transition={{
          duration: 30,
          repeat: Infinity,
          repeatType: 'reverse',
          ease: 'easeInOut',
        }}
      />
      <div className="absolute inset-0 bg-[url('data:image/svg+xml,%3Csvg%20width=%2240%22%20height=%2240%22%20viewBox=%220%200%2040%2040%22%20xmlns=%22http://www.w3.org/2000/svg%22%3E%3Cg%20fill=%22%23ffffff%22%20fill-opacity=%220.05%22%3E%3Cpath%20d=%22M0%2038.59l2.83-2.83%201.41%201.41L1.41%2040H0v-1.41zM0%201.4l2.83%202.83L4.24%202.83%201.41%200H0v1.41zM38.59%2040l-2.83-2.83L34.34%2038.59%2037.17%2040H38.59v-1.41zM40%201.41l-2.83%202.83L35.76%202.83%2038.59%200H40v1.41zM20%2018.6l2.83-2.83%201.41%201.41L21.41%2020l2.83%202.83-1.41%201.41L20%2021.41l-2.83%202.83-1.41-1.41L18.59%2020l-2.83-2.83%201.41-1.41L20%2018.6z%22/%3E%3C/g%3E%3C/svg%3E')]"></div>
      <div className="absolute bottom-8 right-8 opacity-20 text-shadow">
        <div className="text-6xl font-bold text-white">MyOS</div>
      </div>
    </div>
  );
};

export default Wallpaper;
