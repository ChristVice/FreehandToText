import numpy as np
import tensorflow as tf


def char_mappings():
    chars = [' ', '!', '"', '#', '&', "'", '(', ')', '*', '+', ',', '-', '.', '/', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', ':', ';', '?', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z']
    char_to_int = {char: i for i, char in enumerate(chars)}
    int_to_char = {i: char for i, char in enumerate(chars)}
    return char_to_int, int_to_char

def get_decoded_texts(raw_predictions: np.ndarray) -> list[str]:
    _, int_to_char = char_mappings()
    """Converts raw model predictions to decoded text strings."""
    raw_preds_tensor = tf.convert_to_tensor(raw_predictions, dtype=tf.float32)
    batch_size = tf.shape(raw_preds_tensor)[0]

    # transpose to give to CTC decoder
    transposed_preds = tf.transpose(raw_preds_tensor, perm=[1, 0, 2])
    input_length = tf.fill([batch_size], tf.shape(raw_preds_tensor)[1])

    # Decode
    decoded_preds, _ = tf.nn.ctc_beam_search_decoder(transposed_preds, input_length, beam_width=10, top_paths=1)
    decoded_preds_dense = tf.sparse.to_dense(decoded_preds[0], default_value=-1)

    # Convert predictions to text
    def decode_prediction(preds):
        return ''.join(int_to_char[p] for p in preds if p != -1 and p != (len(int_to_char)-1))

    predictions_text = [decode_prediction(seq.numpy().tolist()) for seq in decoded_preds_dense]
    return predictions_text