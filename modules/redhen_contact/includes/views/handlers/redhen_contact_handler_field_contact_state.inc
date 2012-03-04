<?php
class redhen_contact_handler_field_contact_state extends views_handler_field {
  function render($values) {
    if ($values->redhen_contact_redhen_state == REDHEN_STATE_ACTIVE) {
      $text = 'active';
    }
    elseif ($values->redhen_contact_redhen_state == REDHEN_STATE_ARCHIVED) {
      $text = 'archived';
    }
    return $text;
  }
}