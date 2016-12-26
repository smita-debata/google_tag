<?php
/**
 * @file
 * Documents hooks provided by this module.
 *
 * @author Jim Berry ("solotandem", http://drupal.org/user/240748)
 */

/**
 * @addtogroup hooks
 * @{
 */

/**
 * Alters the state of snippet insertion on the current page response.
 *
 * This hook allows other modules to alter the state of snippet insertion based
 * on custom conditions that cannot be defined by the status, path, and role
 * conditions provided by this module.
 *
 * @param string $satisfied
 *   The snippet insertion state.
 */
function hook_google_tag_insert_alter(&$satisfied) {
  // Do something to the state.
  $state = !$state;
}

/**
 * Alters the snippets to be inserted on the current page response.
 *
 * This hook allows other modules to alter the snippets to be inserted based on
 * custom settings not defined by this module.
 *
 * @param string $script
 *   The script snippet.
 * @param string $noscript
 *   The noscript snippet.
 */
function hook_google_tag_snippet_alter(&$script, &$noscript) {
  // Do something to the script snippet.
  $script = str_replace('insertBefore', 'insertAfter', $script);
}
