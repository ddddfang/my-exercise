import numpy as np
#import tensorflow as tf

# use tensorflow 1.x ????
import tensorflow.compat.v1 as tf
tf.disable_v2_behavior()


def nchw2nhwc(nchw_tensor):
    # n(0) c(1) h(2) w(3)
    return tf.transpose(nchw_tensor, [0, 2, 3, 1])

def nhwc2nchw(nhwc_tensor):
    # n(0) h(1) w(2) c(3)
    return tf.transpose(nhwc_tensor, [0, 3, 1, 2])

def hwio2nchw(hwio_tensor):
    return tf.transpose(hwio_tensor, [3, 2, 0, 1])

input_data = [i for i in range(25)]    #python列表
input_data = np.asarray(input_data)     #numpy array
input_data = input_data.reshape(1,5,5,1) #nhwc
input_data = input_data.astype(np.float32)


filter_data = [i for i in range(16)]   #python列表
filter_data = np.asarray(filter_data)    #numpy array
filter_data = filter_data.reshape(4,4,1,1) # H, W, Input_c, Output_c
filter_data = filter_data.astype(np.float32)


input_tensor = tf.constant(input_data, dtype=tf.float32)
filter_tensor = tf.constant(filter_data, dtype=tf.float32)
#output_tensor = tf.nn.conv2d(input_tensor, filter_tensor, strides=[1,2,2,1], padding="SAME")
output_tensor = tf.nn.conv2d(input_tensor, filter_tensor, strides=[1,2,2,1], padding="SAME",dilations=[1,2,2,1])
#print(input_tensor)
#print(filter_tensor)
#print(output_tensor)

init_op = tf.global_variables_initializer()
#init_op = tf.initialize_all_variables()

with tf.Session() as sess:
    sess.run(init_op)
    result = sess.run(output_tensor)

    input_data = nhwc2nchw(input_data)
    print(tf.shape(input_data))
    print(sess.run(input_data))

    filter_data = hwio2nchw(filter_data)
    print(tf.shape(filter_data))
    print(sess.run(filter_data))

    result = nhwc2nchw(result)
    print(tf.shape(result))
    print(sess.run(result))




a = np.arange(30).reshape(2, 3, 5)


