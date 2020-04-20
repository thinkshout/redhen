<?php

/**
 * @file
 * Contains redhen_contact.page.inc..
 *
 * Page callback for Contact entities.
 */

use Drupal\Core\Render\Element;
use Drupal\Core\Link;
use Drupal\Core\Url;

/**
 * Prepares variables for Contact templates.
 *
 * Default template: redhen_contact.html.twig.
 *
 * @param array $variables
 *   An associative array containing:
 *   - elements: An associative array containing the user information and any
 *   - attributes: HTML attributes for the containing element.
 */
function template_preprocess_redhen_contact(array &$variables) {
  // Fetch Contact Entity Object.
  $redhen_contact = $variables['elements']['#redhen_contact'];

  // Helpful $content variable for templates.
  foreach (Element::children($variables['elements']) as $key) {
    $variables['content'][$key] = $variables['elements'][$key];
  }
}

/**
* Prepares variables for a custom entity type creation list templates.
*
* Default template: redhen_contact-content-add-list.html.twig.
*
* @param array $variables
*   An associative array containing:
*   - content: An array of redhen_contact-types.
*
* @see block_content_add_page()
*/
function template_preprocess_redhen_contact_content_add_list(&$variables) {
  $variables['types'] = [];
  $query = \Drupal::request()->query->all();
  foreach ($variables['content'] as $type) {
    $variables['types'][$type->id()] = [
      'link' => Link::fromTextAndUrl($type->label(), new Url('entity.redhen_contact.add_form', [
        'redhen_contact_type' => $type->id()
      ], ['query' => $query])),
      'description' => [
      '#markup' => $type->label(),
      ],
      'title' => $type->label(),
      'localized_options' => [
      'query' => $query,
      ],
    ];
  }
}
