<?php

/**
 * @file
 * Definition of redhen_contact_views_handler_field_redhen_contact_link_delete.
 */

/**
 * Field handler to present the delete link for a RedHen Contact.
 *
 * @ingroup views_field_handlers
 */
class redhen_contact_views_handler_field_redhen_contact_link_delete extends redhen_contact_views_handler_field_redhen_contact_link {

  function render_link($contact, $values) {
    if (entity_access('delete', 'redhen_contact', $contact)) {
      $this->options['alter']['make_link'] = TRUE;
      $this->options['alter']['path'] = 'redhen/contact/' . $contact->contact_id . '/view/delete';
      $text = !empty($this->options['text']) ? $this->options['text'] : t('Delete');
      return $text;
    }
  }
}
