<?php

/**
 * @file
 * Contains redhen_org.page.inc..
 *
 * Page callback for Org entities.
 */

use Drupal\Core\Render\Element;
use Drupal\Core\Link;
use Drupal\Core\Url;

/**
 * Prepares variables for Org templates.
 *
 * Default template: redhen_org.html.twig.
 *
 * @param array $variables
 *   An associative array containing:
 *   - elements: An associative array containing the user information and any
 *   - attributes: HTML attributes for the containing element.
 */
function template_preprocess_redhen_org(array &$variables) {
  // Fetch Org Entity Object.
  $redhen_org = $variables['elements']['#redhen_org'];

  // Helpful $content variable for templates.
  foreach (Element::children($variables['elements']) as $key) {
    $variables['content'][$key] = $variables['elements'][$key];
  }
}

/**
* Prepares variables for a custom entity type creation list templates.
*
* Default template: redhen_org-content-add-list.html.twig.
*
* @param array $variables
*   An associative array containing:
*   - content: An array of redhen_org-types.
*
* @see block_content_add_page()
*/
function template_preprocess_redhen_org_content_add_list(&$variables) {
  $variables['types'] = [];
  $query = \Drupal::request()->query->all();
  foreach ($variables['content'] as $type) {
    $variables['types'][$type->id()] = [
      'link' => Link::fromTextAndUrl($type->label(), new Url('entity.redhen_org.add_form', [
        'redhen_org_type' => $type->id()
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
