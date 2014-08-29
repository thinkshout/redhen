<?php

/**
 * @file
 * Definition of redhen_contact_views_handler_field_redhen_contact_link_edit.
 */

/**
 * Field handler to present the edit link for a RedHen Contact.
 *
 * @ingroup views_field_handlers
 */
class redhen_contact_views_handler_field_redhen_contact_link_edit extends redhen_contact_views_handler_field_redhen_contact_link {

  function render_link($contact, $values) {
    if (entity_access('edit', 'redhen_contact', $contact)) {
      $this->options['alter']['make_link'] = TRUE;
      $this->options['alter']['path'] = 'redhen/contact/' . $contact->contact_id . '/view/edit';
      $text = !empty($this->options['text']) ? $this->options['text'] : t('Edit');
      return $text;
    }
  }
}
